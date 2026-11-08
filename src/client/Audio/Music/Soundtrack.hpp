#pragma once

#include <optional>
#include "IMusicPlayer.hpp"
#include "SoundtrackParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Which music plays: the theme outside a game, the music of a game
 * once a game starts. Entering a game stops the theme at once and starts the
 * music of the game GAME_MUSIC_SILENCE later; a title screen stops the music
 * of the game and plays the theme again.
 */
class Soundtrack {
 public:
  explicit Soundtrack(SoundtrackParts parts);

  /** @brief Stops the music of a game, even before it starts, and plays the
   * theme, unless it plays already. */
  void playTheme();

  /** @brief Stops the theme and starts the music of a game GAME_MUSIC_SILENCE
   * later, unless it plays already or is about to. */
  void playGameMusic();

  /** @brief Stops the theme and the music of a game, and starts the music of
   * the boss BOSS_ALARM_DURATION later, unless it plays already or is about
   * to. */
  void playBossMusic();

  /** @brief Stops the music of a game at once and cancels the silence before
   * it; the theme is left as it is. */
  void silenceMusic();

  /** @brief Moves the waits before the music of a game and the music of the
   * boss on by @p elapsed, and starts each music once its wait is over. */
  void advance(engine::Duration elapsed);

 private:
  /** @brief Cancels the wait before the music of the boss and stops it. */
  void stopBoss();

  IMusicPlayer* theme_;
  IMusicPlayer* gameMusic_;
  IMusicPlayer* bossMusic_;
  std::optional<engine::Duration> silenceLeft_;
  std::optional<engine::Duration> alarmLeft_;
};

}  // namespace rtype::client
