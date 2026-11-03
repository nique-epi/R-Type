#pragma once

#include <optional>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Where the music is in its file: what the intro follows, so each of
 * its pictures lands on its beat. */
class IMusicClock {
 public:
  virtual ~IMusicClock() = default;

  /** @returns How far the music has played since its first beat, negative
   * before it, or nothing while it does not play: no music file, or not
   * started yet. */
  [[nodiscard]] virtual std::optional<engine::Duration> playingOffset()
      const = 0;
};

}  // namespace rtype::client
