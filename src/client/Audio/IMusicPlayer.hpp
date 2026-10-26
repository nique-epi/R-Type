#pragma once

#include "IMusicClock.hpp"

namespace rtype::client {

/** @brief A music the client starts and stops: the theme, which the intro
 * follows and starts itself once its black second is over, so its first
 * beat lands on the time 0 of the intro, and the music of a game. */
class IMusicPlayer : public IMusicClock {
 public:
  /** @brief Starts the music from the start of its file, unless it plays
   * already or has no file. */
  virtual void start() = 0;

  /** @brief Stops the music: start() then plays it from the start of its
   * file again. */
  virtual void stop() = 0;
};

}  // namespace rtype::client
