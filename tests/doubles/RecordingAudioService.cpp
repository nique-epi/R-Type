#include "RecordingAudioService.hpp"
#include <optional>
#include <string>
#include <string_view>
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

void RecordingAudioService::playGameMusic(
    const std::optional<std::string>& levelMusic) {
  musicCalls_.push_back(levelMusic.has_value() ? MusicCall::LevelMusic
                                               : MusicCall::GameMusic);
}

void RecordingAudioService::silenceMusic() {
  musicCalls_.push_back(MusicCall::Silence);
}

void RecordingAudioService::playBossMusic(
    [[maybe_unused]] std::string_view music) {
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
