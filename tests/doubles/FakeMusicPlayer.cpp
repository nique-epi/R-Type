#include "FakeMusicPlayer.hpp"
#include <optional>
#include "AudioConstants.hpp"
#include "TimeConstants.hpp"

void FakeMusicPlayer::start() {
  if (fileRemoved_ || clock_.playingOffset().has_value()) {
    return;
  }
  clock_.setPlayingOffset(-rtype::client::THEME_FIRST_BEAT);
}

void FakeMusicPlayer::stop() { clock_.setPlayingOffset(std::nullopt); }

std::optional<rtype::engine::Duration> FakeMusicPlayer::playingOffset() const {
  return clock_.playingOffset();
}

void FakeMusicPlayer::setPlayingOffset(
    std::optional<rtype::engine::Duration> offset) {
  clock_.setPlayingOffset(offset);
}

void FakeMusicPlayer::removeTheFile() { fileRemoved_ = true; }
