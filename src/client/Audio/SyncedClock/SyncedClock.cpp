#include "SyncedClock.hpp"
#include <algorithm>
#include <chrono>
#include <optional>
#include <ratio>
#include "AudioConstants.hpp"
#include "IMusicClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The share of its gap a frame of @p elapsed closes: elapsed /
 * MUSIC_CATCH_UP_TIME, the whole gap for a frame that long or longer. */
float catchUpShareOf(engine::Duration elapsed) {
  return std::min(1.0F, std::chrono::duration<float>(elapsed) /
                            std::chrono::duration<float>(MUSIC_CATCH_UP_TIME));
}

/** @returns @p share of @p gap, rounded to the nearest duration: the share is
 * computed in float. */
engine::Duration catchUpOf(engine::Duration gap, float share) {
  return std::chrono::round<engine::Duration>(
      std::chrono::duration<float, std::nano>(gap) * share);
}

}  // namespace

SyncedClock::SyncedClock(const IMusicClock& music) : music_(&music) {}

void SyncedClock::advance(engine::Duration elapsed) {
  time_ += elapsed;
  const std::optional<engine::Duration> gap = gapToMusic();
  if (!gap.has_value()) {
    return;
  }
  time_ += std::chrono::abs(*gap) > MUSIC_SYNC_SNAP
               ? *gap
               : catchUpOf(*gap, catchUpShareOf(elapsed));
}

void SyncedClock::jumpTo(engine::Duration time) {
  const std::optional<engine::Duration> offset = music_->playingOffset();
  shift_ = offset.has_value() ? time - *offset : engine::Duration::zero();
  time_ = time;
}

engine::Duration SyncedClock::now() const { return time_; }

std::optional<engine::Duration> SyncedClock::gapToMusic() const {
  const std::optional<engine::Duration> offset = music_->playingOffset();
  if (!offset.has_value()) {
    return std::nullopt;
  }
  return *offset + shift_ - time_;
}

}  // namespace rtype::client
