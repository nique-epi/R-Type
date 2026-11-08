#include "Soundtrack.hpp"
#include <optional>
#include "AudioConstants.hpp"
#include "IMusicPlayer.hpp"
#include "SoundtrackParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Moves @p wait on by @p elapsed and clears it once over.
 * @returns Whether it is over now. */
bool waitOver(std::optional<engine::Duration>& wait, engine::Duration elapsed) {
  if (!wait.has_value()) {
    return false;
  }
  *wait -= elapsed;
  if (*wait > engine::Duration::zero()) {
    return false;
  }
  wait.reset();
  return true;
}

}  // namespace

Soundtrack::Soundtrack(SoundtrackParts parts)
    : theme_(&parts.theme.get()),
      gameMusic_(&parts.gameMusic.get()),
      bossMusic_(&parts.bossMusic.get()) {}

void Soundtrack::playTheme() {
  silenceLeft_.reset();
  stopBoss();
  gameMusic_->stop();
  theme_->start();
}

void Soundtrack::playGameMusic() {
  stopBoss();
  if (silenceLeft_.has_value() || gameMusic_->playingOffset().has_value()) {
    return;
  }
  theme_->stop();
  silenceLeft_ = GAME_MUSIC_SILENCE;
}

void Soundtrack::playBossMusic() {
  if (alarmLeft_.has_value() || bossMusic_->playingOffset().has_value()) {
    return;
  }
  silenceLeft_.reset();
  theme_->stop();
  gameMusic_->stop();
  alarmLeft_ = BOSS_ALARM_DURATION;
}

void Soundtrack::silenceMusic() {
  silenceLeft_.reset();
  gameMusic_->stop();
}

void Soundtrack::stopBoss() {
  alarmLeft_.reset();
  bossMusic_->stop();
}

void Soundtrack::advance(engine::Duration elapsed) {
  if (waitOver(silenceLeft_, elapsed)) {
    gameMusic_->start();
  }
  if (waitOver(alarmLeft_, elapsed)) {
    bossMusic_->start();
  }
}

}  // namespace rtype::client
