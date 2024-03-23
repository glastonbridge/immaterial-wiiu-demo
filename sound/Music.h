#pragma once

#include <minivorbis.h>
#include <fstream>
#include <vector>
#include <sndcore2/voice.h>

// only one of these can exist at a time Or Else, TODO make
// a getter
struct MusicPlayer {
  MusicPlayer(const char* oggFileName, float offsetSeconds = 0.0);
  ~MusicPlayer();
  void play();
  void pause();
  void pauseToggle(void);
  bool isPlaying();
  void seek(float seconds);
  float currentTime();
  
  protected:
    void frameCallback(void);

  private:
    bool playState;
    AXVoice* voiceLeft;
    AXVoice* voiceRight;
    uint32_t sampleRate;
    int16_t* bufferLeft;
    int16_t* bufferRight;
};
