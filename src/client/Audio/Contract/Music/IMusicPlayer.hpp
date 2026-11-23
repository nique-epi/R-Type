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
  /** @brief Pauses a music that plays: start() then plays it on from where
   * it was. A music that does not play is left as it is. */
  virtual void pause() = 0;
  /** @brief Scales the volume of the music by @p level, from
   * MUSIC_FADE_SILENT to MUSIC_FADE_FULL, until the next call; a music
   * plays at MUSIC_FADE_FULL until then. */
  virtual void setFade(float level) = 0;
};

}  // namespace rtype::client
