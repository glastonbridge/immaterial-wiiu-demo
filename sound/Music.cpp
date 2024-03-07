#include <minivorbis.h>
#include "Music.h"
#include <whb/log.h>
#include <coreinit/memdefaultheap.h>
#include <sndcore2/core.h>
#include <coreinit/cache.h>
#include <cstring>

MusicPlayer::MusicPlayer(const char* oggFileName) {
    AXInitParams init = {AX_INIT_RENDERER_48KHZ, 0, 0}; // possible sample rates are 48k and 32k, rest of parameters doesn't matter
    AXInitWithParams(&init);

    // Set up ogg decode
    FILE* audioFile = fopen(oggFileName, "rb");
    OggVorbis_File vorbisDecoderState;
    if (ov_open_callbacks(audioFile, &vorbisDecoderState, NULL, 0, OV_CALLBACKS_DEFAULT) != 0) {
        WHBLogPrintf("Ogg reading failed");
    }

    vorbis_info* info = ov_info(&vorbisDecoderState, -1);
    sampleRate = info->rate;

    // Read ogg data
    // TODO put in thread
    long readSize;
    std::vector<int16_t> decodedData;
    int16_t pcmout[2048]; // Temporary buffer for decoded data, half the size since int16_t is 2 bytes
    int section = 0;    
    do {
        readSize = ov_read(&vorbisDecoderState, (char*)pcmout, sizeof(pcmout), 1, 2, 1, &section);
        for (int i = 0; i < readSize / sizeof(int16_t); i++) {
            decodedData.push_back(pcmout[i]);
        }
    } while (readSize > 0);

    // Copy interleaved data to separate buffers.
    bufferLeft = (int16_t*)(MEMAllocFromDefaultHeap(decodedData.size() / 2 * sizeof(int16_t)));
    bufferRight = (int16_t*)(MEMAllocFromDefaultHeap(decodedData.size() / 2 * sizeof(int16_t)));
    for (unsigned int i = 0; i < decodedData.size(); i += 2) {
        bufferLeft[i / 2] = decodedData[i];
        bufferRight[i / 2] = decodedData[i + 1];
    }

    // Flush
    DCFlushRange(bufferLeft, decodedData.size() / 2 * sizeof(int16_t));
    DCFlushRange(bufferRight, decodedData.size() / 2 * sizeof(int16_t));
    WHBLogPrintf("Loaded data: %d samples at rate %d", decodedData.size() / 2, sampleRate);

    // If we batch read, closing here is fine
    ov_clear(&vorbisDecoderState);
    fclose(audioFile);

    // Init the playback
    voiceLeft = AXAcquireVoice(
        31, // Priority, higher = more important. doesn't matter for us. 31 is max, in any case.
        NULL, // Status callback. We don't care about this.
        NULL // User data
    );
    voiceRight = AXAcquireVoice(31, NULL, NULL);
    
    // Lets go Full Crime: no callback just dump the entire buffer at the DSP in one go
    // I *think* what you would do with the callback is have the buffer loop and then fill in the audio callback
    // if you go past halfway / past end, pingponging between halves (and make the buffer 6ms since audio callback)
    // is fixed to 3ms (?????) in cafe
    // TODO: figure that out once noise is being made
    AXVoiceOffsets bufferInfo;
    bufferInfo.dataType = AX_VOICE_FORMAT_LPCM16;
    bufferInfo.loopingEnabled = AX_VOICE_LOOP_DISABLED;
    bufferInfo.loopOffset = 0;
    bufferInfo.endOffset = decodedData.size() / 2;
    bufferInfo.currentOffset = 0;
    bufferInfo.data = bufferLeft;
    AXSetVoiceOffsets(voiceLeft, &bufferInfo);

    // Reuse info because lazy (this memory is not kept by AX)
    bufferInfo.data = bufferRight;
    AXSetVoiceOffsets(voiceRight, &bufferInfo);
    
    // Set up pitch bending
    AXSetVoiceSrcType(voiceLeft, AX_VOICE_SRC_TYPE_NONE); // we do not want to pitch bend
    AXSetVoiceSrcRatio(voiceLeft, 1.0f); // ignored
    AXSetVoiceSrcType(voiceRight, AX_VOICE_SRC_TYPE_NONE); // we do not want to pitch bend
    AXSetVoiceSrcRatio(voiceRight, 1.0f); // ignored

    // Set up volume
    AXVoiceVeData volumeInfo = {
        0x8000, // Volume actual. These are fixedpoint 1.15 values, so 0x8000 is 1.0
        0 // Volume delta (added to actual for each sample in the next 3ms frame)
    };
    AXSetVoiceVe(voiceLeft, &volumeInfo);
    AXSetVoiceVe(voiceRight, &volumeInfo);

    // Set up mixing
    // There are (for TV) six channels, with four "buses", this structure contains volume / delta for each
    AXVoiceDeviceMixData deviceVolumeData[6];
    memset(&deviceVolumeData, 0, sizeof(deviceVolumeData));
    deviceVolumeData[0].bus[0].volume = 0x8000; // Left channel
    AXSetVoiceDeviceMix(voiceLeft, AX_DEVICE_TYPE_TV, 0, (AXVoiceDeviceMixData*)&deviceVolumeData);
    deviceVolumeData[0].bus[0].volume = 0; // Left channel
    deviceVolumeData[1].bus[0].volume = 0x8000; // Right channel
    AXSetVoiceDeviceMix(voiceRight, AX_DEVICE_TYPE_TV, 0, (AXVoiceDeviceMixData*)&deviceVolumeData);


    // AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, &deviceVolumeData); // <-- uncomment for gamepad

    // Wiimotes (TODO, WUT doesn't define these)
    //AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_RMT, 0, 0, &deviceVolumeData); // <-- uncomment for wiimote 0 
    //AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_RMT, 0, 1, &deviceVolumeData); // <-- uncomment for wiimote 1
    
    AXSetVoiceCurrentOffset(voiceLeft, 0); // start at beginning
    AXSetVoiceState(voiceLeft, AX_VOICE_STATE_STOPPED);
    AXSetVoiceCurrentOffset(voiceRight, 0);
    AXSetVoiceState(voiceRight, AX_VOICE_STATE_STOPPED);
}

MusicPlayer::~MusicPlayer() {
    AXFreeVoice(voiceLeft);
    AXFreeVoice(voiceRight);
    MEMFreeToDefaultHeap(bufferLeft);
    MEMFreeToDefaultHeap(bufferRight);
}

void MusicPlayer::play() {
    AXSetVoiceState(voiceLeft, AX_VOICE_STATE_PLAYING);
    AXSetVoiceState(voiceRight, AX_VOICE_STATE_PLAYING);
    playState = true;
}

void MusicPlayer::pause() {
    AXSetVoiceState(voiceLeft, AX_VOICE_STATE_STOPPED);
    AXSetVoiceState(voiceRight, AX_VOICE_STATE_STOPPED);
    playState = false;
}

void MusicPlayer::pauseToggle(void) {
    if (playState == true) {
        pause();
    } else {
        play();
    }
}

void MusicPlayer::seek(float seconds) {
    AXVoiceOffsets bufferInfo;
    AXGetVoiceOffsets(voiceLeft, &bufferInfo);
    bufferInfo.currentOffset = (uint32_t)(seconds * sampleRate);
    AXSetVoiceOffsets(voiceLeft, &bufferInfo);
    AXGetVoiceOffsets(voiceRight, &bufferInfo);
    bufferInfo.currentOffset = (uint32_t)(seconds * sampleRate);
    AXSetVoiceOffsets(voiceRight, &bufferInfo);
}

bool MusicPlayer::isPlaying() {
    return playState;
}

float MusicPlayer::currentTime() {
    AXVoiceOffsets bufferInfo;
    AXGetVoiceOffsets(voiceLeft, &bufferInfo);
    return (float)bufferInfo.currentOffset / (float)sampleRate;
}
