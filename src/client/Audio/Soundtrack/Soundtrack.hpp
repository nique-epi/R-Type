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

  /** @brief Moves the silence before the music of a game on by
   * @p elapsed, and starts that music once the silence is over. */
  void advance(engine::Duration elapsed);

 private:
  IMusicPlayer* theme_;
  IMusicPlayer* gameMusic_;
  std::optional<engine::Duration> silenceLeft_;
};

}  // namespace rtype::client
