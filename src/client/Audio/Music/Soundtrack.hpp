#pragma once

#include <optional>
#include "IMusicPlayer.hpp"
#include "MusicFade.hpp"
#include "SoundtrackParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Which music plays: the theme outside a game, the music of a game
 * once a game starts. Entering a game stops the theme at once and starts the
 * music of the game GAME_MUSIC_SILENCE later; a title screen stops the music
 * of the game and plays the theme again. A screen with a music of its own
 * pauses the theme for as long as it shows. A level may name a music of its
 * own (useLevelMusic()): it then plays in place of the music of a game, from
 * then to the next title screen; the alarm of a boss stops it, and it comes
 * back once the boss is gone, or after the alarm for a boss with no music.
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

  /** @brief Stops the theme and the music of a game, then fades a music in
   * over the last BOSS_MUSIC_FADE_IN of the alarm, BOSS_ALARM_DURATION long:
   * the music of the boss when @p named says its sheet names one, or when the
   * level plays no music of its own; the music of the level otherwise. While
   * the alarm or the music of the boss plays, it only notes @p named. Over a
   * music of the level that plays again after the alarm, a boss that names
   * its music stops it and starts its own at once. */
  void playBossMusic(bool named);

  /** @brief Stops the music of a game at once, the music a level names
   * included, and cancels the silence before it; the theme is left as it is,
   * and playGameMusic() brings the music of the level back. */
  void silenceMusic();
  /** @brief Pauses the theme when it plays, for a screen that plays a music
   * of its own; playTheme() then leaves it paused. */
  void pauseTheme();
  /** @brief Plays the theme on from where pauseTheme() left it; nothing when
   * it was not paused, or when a game stopped it since. */
  void resumeTheme();

  /** @brief Moves the waits before the music of a game and the music of the
   * boss, and the fade of the latter, on by @p elapsed, and starts each music
   * once its wait is over. */
  void advance(engine::Duration elapsed);

  /** @brief Plays @p music in place of the music of a game until the next
   * playTheme(): at once when the music of a game plays, at the end of the
   * silence when it is about to; nothing when it plays already. */
  void useLevelMusic(IMusicPlayer& music);

 private:
  /** @returns The music of a game that plays: the level's own, or the music
   * of a game. */
  [[nodiscard]] IMusicPlayer& gameMusic() const;

  /** @brief Cancels the wait before the music of the boss and stops it. */
  void stopBoss();

  IMusicPlayer* theme_;
  IMusicPlayer* gameMusic_;
  IMusicPlayer* bossMusic_;
  IMusicPlayer* levelMusic_{nullptr};
  std::optional<engine::Duration> silenceLeft_;
  std::optional<engine::Duration> alarmLeft_;
  MusicFade fade_;
  /** @brief Whether the sheet of the boss of the alarm names its music. */
  bool bossNamed_{false};
  /** @brief Whether pauseTheme() paused a theme that played, and nothing
   * stopped it since. */
  bool themePaused_{false};
};

}  // namespace rtype::client
