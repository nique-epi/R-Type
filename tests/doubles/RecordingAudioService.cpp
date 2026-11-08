#include "RecordingAudioService.hpp"
#include <vector>
#include "Ambience.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
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
}

void RecordingAudioService::playGameMusic() {
  musicCalls_.push_back(MusicCall::GameMusic);
}

void RecordingAudioService::silenceMusic() {
  musicCalls_.push_back(MusicCall::Silence);
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
