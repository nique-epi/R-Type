#include "FakeMusicClock.hpp"
#include <optional>
#include "TimeConstants.hpp"

std::optional<rtype::engine::Duration> FakeMusicClock::playingOffset() const {
  return offset_;
}

void FakeMusicClock::setPlayingOffset(
    std::optional<rtype::engine::Duration> offset) {
  offset_ = offset;
}
