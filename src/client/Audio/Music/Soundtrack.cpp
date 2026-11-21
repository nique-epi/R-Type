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

/** @returns @p levelPlayer, a music of the level, when there is one, else
 * @p fallback. */
IMusicPlayer& chosen(IMusicPlayer* levelPlayer, IMusicPlayer& fallback) {
  return levelPlayer != nullptr ? *levelPlayer : fallback;
}

}  // namespace

Soundtrack::Soundtrack(SoundtrackParts parts)
    : theme_(&parts.theme.get()),
      gameMusic_(&parts.gameMusic.get()),
      bossMusic_(&parts.bossMusic.get()) {}

void Soundtrack::playTheme() {
  silenceLeft_.reset();
  stopBoss();
  chosen(levelMusic_, *gameMusic_).stop();
  levelMusic_ = nullptr;
  levelBossMusic_ = nullptr;
  if (!themePaused_) {
    theme_->start();
  }
}

void Soundtrack::playGameMusic() {
  stopBoss();
  if (silenceLeft_.has_value() ||
      chosen(levelMusic_, *gameMusic_).playingOffset().has_value()) {
    return;
  }
  theme_->stop();
  themePaused_ = false;
  silenceLeft_ = GAME_MUSIC_SILENCE;
}

void Soundtrack::playBossMusic() {
  if (alarmLeft_.has_value() ||
      chosen(levelBossMusic_, *bossMusic_).playingOffset().has_value()) {
    return;
  }
  silenceLeft_.reset();
  theme_->stop();
  themePaused_ = false;
  chosen(levelMusic_, *gameMusic_).stop();
  alarmLeft_ = BOSS_ALARM_DURATION;
}

bool Soundtrack::silenceMusic() {
  if (levelMusic_ != nullptr && levelBossMusic_ == nullptr) {
    return false;
  }
  silenceLeft_.reset();
  chosen(levelMusic_, *gameMusic_).stop();
  return true;
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
  chosen(levelBossMusic_, *bossMusic_).stop();
}

void Soundtrack::advance(engine::Duration elapsed) {
  if (waitOver(silenceLeft_, elapsed)) {
    chosen(levelMusic_, *gameMusic_).start();
  }
  if (waitOver(alarmLeft_, elapsed)) {
    chosen(levelBossMusic_, *bossMusic_).start();
  }
}

void Soundtrack::useLevelMusic(IMusicPlayer& music, IMusicPlayer* bossMusic) {
  levelBossMusic_ = bossMusic;
  if (levelMusic_ == &music) {
    return;
  }
  const bool playing =
      chosen(levelMusic_, *gameMusic_).playingOffset().has_value();
  chosen(levelMusic_, *gameMusic_).stop();
  levelMusic_ = &music;
  if (playing) {
    music.start();
  }
}

}  // namespace rtype::client
