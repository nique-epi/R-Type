#pragma once

#include "IMusicPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Raises one music from silence to its full volume over a length,
 * as the frames go by: the music that follows the alarm of a boss. */
class MusicFade {
 public:
  /** @brief Starts @p music silent, raised to its full volume over
   * @p length; a fade that ran gives its music its full volume first. */
  void begin(IMusicPlayer& music, engine::Duration length);

  /** @brief Raises the music by @p elapsed of its fade; once the length
   * passed, it plays at its full volume and the fade is over. Nothing
   * without a fade. */
  void advance(engine::Duration elapsed);

  /** @brief Gives the music its full volume at once and ends the fade;
   * nothing without a fade. */
  void cancel();

 private:
  IMusicPlayer* music_{nullptr};
  engine::Duration length_{};
  engine::Duration elapsed_{};
};

}  // namespace rtype::client
