#pragma once

#include <map>
#include <math.h>

#include <sync.h>
#include "../sound/Music.h"

#pragma once

#include <map>
#include <math.h>
#include "../sound/Music.h"

// Global variables declaration
extern float secondsPerRowForCB;

// Function declarations (we need these with these signatures for the callbacks)
void toggleMusicPause(void* musicPlayerP, int pause);
void seekByRow(void* musicPlayerP, int row);
int isPlaying(void* musicPlayerP);

// Sync class definition
class Sync {
public:
  Sync(const char* basePath, const char* syncIP, MusicPlayer* musicPlayer, float secondsPerRow);
  ~Sync();
  void update();
  float v(const char* track);

private:
  const char* syncIP;
  sync_device* rocket;
  MusicPlayer* musicPlayer;
  float secondsPerRow;
  std::map<const char*, const sync_track*> tracks;
  #ifndef SYNC_PLAYER
    sync_cb rocketCallbacks;
  #endif
  const sync_track* getTrack(const char* trackName);
  void connect();
};

void createSyncHandler(const char* basePath, const char* syncIP, MusicPlayer* musicPlayer, float secondsPerRow);
Sync* getSyncHandler();
void destroySyncHandler();
float syncVal(const char* track);
