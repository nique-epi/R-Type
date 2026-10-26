#pragma once

#include <optional>
#include "IMusicClock.hpp"
#include "TimeConstants.hpp"

/** @brief Music clock double whose position the test sets; it plays
 * nothing and needs no audio device. */
class FakeMusicClock : public rtype::client::IMusicClock {
 public:
  [[nodiscard]] std::optional<rtype::engine::Duration> playingOffset()
      const override;

  /** @brief Makes the music play at @p offset, or stop with nothing. */
  void setPlayingOffset(std::optional<rtype::engine::Duration> offset);

 private:
  std::optional<rtype::engine::Duration> offset_;
};
