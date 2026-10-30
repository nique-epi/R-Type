#pragma once

#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the sound module hands to the rest of the client: the player of
 * the short sounds every screen uses, the theme the intro follows, and the
 * choice between the music outside a game and the music of a game.
 *
 * It exists in two kinds: one that plays, and one that plays nothing, so a
 * client launched without sound runs the same code and no screen asks whether
 * the service is there.
 */
class IAudioService {
 public:
  virtual ~IAudioService() = default;

  /** @returns The player of the short sounds of the client. */
  [[nodiscard]] virtual ISoundPlayer& sounds() = 0;

  /** @returns The theme: the music the intro follows and starts. */
  [[nodiscard]] virtual IMusicPlayer& theme() = 0;

  /** @brief Stops the music of a game and plays the theme, unless it plays
   * already. */
  virtual void playTheme() = 0;

  /** @brief Stops the theme and starts the music of a game a silence later,
   * unless it plays already or is about to. */
  virtual void playGameMusic() = 0;

  /** @brief Opens the music files, once the assets folder is known; a music
   * without file stays silent. */
  virtual void open() = 0;

  /** @brief Called each frame: the volumes follow the settings, and the
   * silence before the music of a game moves on by @p elapsed. */
  virtual void update(engine::Duration elapsed) = 0;
};

}  // namespace rtype::client
