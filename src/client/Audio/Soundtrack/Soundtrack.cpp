#include "Soundtrack.hpp"
#include <optional>
#include "AudioConstants.hpp"
#include "IMusicPlayer.hpp"
#include "SoundtrackParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

Soundtrack::Soundtrack(SoundtrackParts parts)
    : theme_(&parts.theme.get()), gameMusic_(&parts.gameMusic.get()) {}

void Soundtrack::playTheme() {
  silenceLeft_.reset();
  gameMusic_->stop();
  theme_->start();
}

void Soundtrack::playGameMusic() {
  if (silenceLeft_.has_value() || gameMusic_->playingOffset().has_value()) {
    return;
  }
  theme_->stop();
  silenceLeft_ = GAME_MUSIC_SILENCE;
}

void Soundtrack::advance(engine::Duration elapsed) {
  if (!silenceLeft_.has_value()) {
    return;
  }
  *silenceLeft_ -= elapsed;
  if (*silenceLeft_ > engine::Duration::zero()) {
    return;
  }
  silenceLeft_.reset();
  gameMusic_->start();
}

}  // namespace rtype::client
