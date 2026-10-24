#pragma once

#include "IMusicClock.hpp"

namespace rtype::client {

/** @brief The music the intro follows and starts itself, once its black
 * second is over, so the first beat of the theme lands on its time 0. */
class IMusicPlayer : public IMusicClock {
 public:
  /** @brief Starts the music from the start of its file, unless it plays
   * already or has no file. */
  virtual void start() = 0;
};

}  // namespace rtype::client
