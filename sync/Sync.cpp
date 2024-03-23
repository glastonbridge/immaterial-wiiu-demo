#include "Sync.h"
#include <sync.h>
#include <whb/log.h>

// Global variables definition
float secondsPerRowForCB = 0.0f;

// Global sync object definition
Sync* syncObjPtr = nullptr;

// Functions implementation
void toggleMusicPause(void* musicPlayerP, int pause) {
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(musicPlayerP);
  if (pause) {
    WHBLogPrintf("Pausing music");
    musicPlayer->pause();
  }
  else {
    WHBLogPrintf("Resuming music");
    musicPlayer->play();
  }
}

void seekByRow(void* musicPlayerP, int row) {
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(musicPlayerP);
  WHBLogPrintf("Seeking to row %d", row);
  musicPlayer->seek(secondsPerRowForCB * row);
}

int isPlaying(void* musicPlayerP) {
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>(musicPlayerP);
  return musicPlayer->isPlaying();
}

// Sync class member functions implementation
Sync::Sync(const char* basePath, const char* syncIP, MusicPlayer* musicPlayer, float secondsPerRow) : syncIP(syncIP), musicPlayer(musicPlayer), secondsPerRow(secondsPerRow) {
  WHBLogPrintf("Creating sync device with track path %s", basePath);
  this->rocket = sync_create_device(basePath);
  #ifndef SYNC_PLAYER
    this->rocketCallbacks.pause = toggleMusicPause;
    this->rocketCallbacks.set_row = seekByRow;
    this->rocketCallbacks.is_playing = isPlaying;
    connect();
  #endif
  update();
  syncObjPtr = this;	
}

Sync::~Sync() {
  sync_destroy_device(rocket);
}

void Sync::update() {
  float row = musicPlayer->currentTime() / secondsPerRow;
  secondsPerRowForCB = secondsPerRow;
  #ifndef SYNC_PLAYER
  WHBLogPrintf("Upd. start");
    if(sync_update(rocket, (int)floor(row), &rocketCallbacks, musicPlayer)) {
      WHBLogPrintf("Sync update failed, reconnecting if in tool mode");
      connect();
    };
    WHBLogPrintf("Upd. end");
  #endif
}

float Sync::v(const char* track) {
  return sync_get_val(getTrack(track), musicPlayer->currentTime() / secondsPerRow);
}

const sync_track* Sync::getTrack(const char* trackName) {
  if (tracks.find(trackName) != tracks.end()) return tracks[trackName];
  const sync_track* track = sync_get_track(rocket, trackName);
  tracks[trackName] = track;
  return track;
}

void Sync::connect() {
  WHBLogPrintf("Connecting to sync server at %s", syncIP);
  #ifndef SYNC_PLAYER
    while (sync_tcp_connect(rocket, syncIP, SYNC_DEFAULT_PORT)) {
      WHBLogPrintf("Failed to connect to sync server, retrying...");
      sync_tcp_connect(rocket, syncIP, SYNC_DEFAULT_PORT);
    }
  #endif
}
