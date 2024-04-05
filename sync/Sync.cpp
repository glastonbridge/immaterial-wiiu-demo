#include "Sync.h"
#include <sync.h>
#include <whb/log.h>

// Global variables definition
float secondsPerRowForCB = 0.0f;

// Functions implementation
void toggleMusicPause(void *musicPlayerP, int pause) {
  MusicPlayer *musicPlayer = static_cast<MusicPlayer *>(musicPlayerP);
  if (pause) {
    WHBLogPrintf("Pausing music");
    musicPlayer->pause();
  } else {
    WHBLogPrintf("Resuming music");
    musicPlayer->play();
  }
}

void seekByRow(void *musicPlayerP, int row) {
  MusicPlayer *musicPlayer = static_cast<MusicPlayer *>(musicPlayerP);
  WHBLogPrintf("Seeking to row %d", row);
  musicPlayer->seek(secondsPerRowForCB * row);
}

int isPlaying(void *musicPlayerP) {
  MusicPlayer *musicPlayer = static_cast<MusicPlayer *>(musicPlayerP);
  return musicPlayer->isPlaying();
}

// Sync class member functions implementation
Sync::Sync(const char *basePath, const char *syncIP, MusicPlayer *musicPlayer,
           float secondsPerRow)
    : syncIP(syncIP), musicPlayer(musicPlayer), secondsPerRow(secondsPerRow) {
  WHBLogPrintf("Creating sync device with track path %s", basePath);

  // saves on a bit of rehashing
  this->tracks.reserve(20);
  this->rocket = sync_create_device(basePath);
#ifndef SYNC_PLAYER
  this->rocketCallbacks.pause = toggleMusicPause;
  this->rocketCallbacks.set_row = seekByRow;
  this->rocketCallbacks.is_playing = isPlaying;
  connect();
#endif
  update();
}

Sync::~Sync() { sync_destroy_device(rocket); }

void Sync::update() {
  float row = musicPlayer->currentTime() / secondsPerRow;
  secondsPerRowForCB = secondsPerRow;
#ifndef SYNC_PLAYER
  // WHBLogPrintf("Upd. start");
  if (sync_update(rocket, (int)floor(row), &rocketCallbacks, musicPlayer)) {
    WHBLogPrintf("Sync update failed, reconnecting if in tool mode");
    connect();
  };
    // WHBLogPrintf("Upd. end");
#endif
}

float Sync::v(const char *track) {
  return sync_get_val(getTrack(track),
                      musicPlayer->currentTime() / secondsPerRow);
}

const sync_track *Sync::getTrack(const char *trackName) {
  auto const found = tracks.try_emplace(trackName);
  if (!found.second) {
    return found.first->second;
  }
  auto *track = sync_get_track(rocket, trackName);
  found.first->second = track;
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

// Global sync object
static Sync *syncHandler = nullptr;
void createSyncHandler(const char *basePath, const char *syncIP,
                       MusicPlayer *musicPlayer, float secondsPerRow) {
  if (syncHandler == nullptr) {
    syncHandler = new Sync(basePath, syncIP, musicPlayer, secondsPerRow);
  }
}

Sync *getSyncHandler() { return syncHandler; }

void destroySyncHandler() {
  if (syncHandler != nullptr) {
    delete syncHandler;
    syncHandler = nullptr;
  }
}

float syncVal(const char *track) {
  if (syncHandler == nullptr)
    return 0.0f;
  return syncHandler->v(track);
}