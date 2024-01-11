#pragma once

#include <minivorbis.h>
#include <fstream>
#include <vector>
#include <sndcore2/voice.h>

// only one of these can exist at a time Or Else, TODO make
// a getter
struct MusicPlayer {
  MusicPlayer(const char* oggFileName);
  ~MusicPlayer();
  void play();
  void pause();
  void seek(float seconds);
  float currentTime();
  
  protected:
    void frameCallback(void);

  private:
    AXVoice* voice;
    uint32_t sampleRate;
    int16_t* bufferLeft;
    int16_t* bufferRight;
};
