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
  gameMusic().stop();
  levelMusic_ = nullptr;
  if (!themePaused_) {
    theme_->start();
  }
}

void Soundtrack::playGameMusic() {
  stopBoss();
  if (silenceLeft_.has_value() || gameMusic().playingOffset().has_value()) {
    return;
  }
  theme_->stop();
  themePaused_ = false;
  silenceLeft_ = GAME_MUSIC_SILENCE;
}

void Soundtrack::playBossMusic(bool named) {
  if (alarmLeft_.has_value() || bossMusic_->playingOffset().has_value()) {
    bossNamed_ = bossNamed_ || named;
    return;
  }
  silenceLeft_.reset();
  theme_->stop();
  themePaused_ = false;
  if (named && levelMusic_ != nullptr &&
      levelMusic_->playingOffset().has_value()) {
    fade_.cancel();
    levelMusic_->stop();
    bossMusic_->start();
    return;
  }
  gameMusic().stop();
  bossNamed_ = named;
  alarmLeft_ = BOSS_ALARM_DURATION - BOSS_MUSIC_FADE_IN;
}

void Soundtrack::silenceMusic() {
  silenceLeft_.reset();
  gameMusic().stop();
}

void Soundtrack::pauseTheme() {
  if (themePaused_ || !theme_->playingOffset().has_value()) {
    return;
  }
  theme_->pause();
  themePaused_ = true;
}

void Soundtrack::resumeTheme() {
  if (!themePaused_) {
    return;
  }
  themePaused_ = false;
  theme_->start();
}

void Soundtrack::stopBoss() {
  alarmLeft_.reset();
  fade_.cancel();
  bossMusic_->stop();
}

void Soundtrack::advance(engine::Duration elapsed) {
  if (waitOver(silenceLeft_, elapsed)) {
    gameMusic().start();
  }
  fade_.advance(elapsed);
  if (waitOver(alarmLeft_, elapsed)) {
    IMusicPlayer& next =
        bossNamed_ || levelMusic_ == nullptr ? *bossMusic_ : *levelMusic_;
    fade_.begin(next, BOSS_MUSIC_FADE_IN);
  }
}

void Soundtrack::useLevelMusic(IMusicPlayer& music) {
  if (levelMusic_ == &music) {
    return;
  }
  const bool playing = gameMusic().playingOffset().has_value();
  gameMusic().stop();
  levelMusic_ = &music;
  if (playing) {
    music.start();
  }
}

IMusicPlayer& Soundtrack::gameMusic() const {
  return levelMusic_ != nullptr ? *levelMusic_ : *gameMusic_;
}

}  // namespace rtype::client
