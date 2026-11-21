#include "RecordingAudioService.hpp"
#include <vector>
#include "Ambience.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "LevelTracks.hpp"
#include "MusicCall.hpp"
#include "TimeConstants.hpp"

RecordingAudioService::RecordingAudioService(
    rtype::client::ISoundPlayer& sounds)
    : sounds_(sounds) {}

rtype::client::ISoundPlayer& RecordingAudioService::sounds() {
  return sounds_.get();
}

rtype::client::IMusicPlayer& RecordingAudioService::theme() { return theme_; }

void RecordingAudioService::playTheme() {
  musicCalls_.push_back(MusicCall::Theme);
  levelMusicNamed_ = false;
  bossMusicNamed_ = false;
}

void RecordingAudioService::playGameMusic(
    const rtype::client::LevelTracks& tracks) {
  if (tracks.music.has_value()) {
    levelMusicNamed_ = true;
    bossMusicNamed_ = tracks.bossMusic.has_value();
  }
  musicCalls_.push_back(tracks.music.has_value() ? MusicCall::LevelMusic
                                                 : MusicCall::GameMusic);
}

bool RecordingAudioService::silenceMusic() {
  musicCalls_.push_back(MusicCall::Silence);
  return !levelMusicNamed_ || bossMusicNamed_;
}

void RecordingAudioService::playBossMusic() {
  musicCalls_.push_back(MusicCall::BossMusic);
}

void RecordingAudioService::setAmbience(rtype::client::Ambience ambience) {
  ambience_ = ambience;
}

rtype::client::Ambience RecordingAudioService::ambience() const {
  return ambience_;
}

void RecordingAudioService::open() {}

void RecordingAudioService::update(
    [[maybe_unused]] rtype::engine::Duration elapsed) {}

const std::vector<MusicCall>& RecordingAudioService::musicCalls() const {
  return musicCalls_;
}
