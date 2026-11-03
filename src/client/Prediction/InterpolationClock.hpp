#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "InterpolationConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Says which server tick the client shows: the one
 * INTERPOLATION_INTERVALS state intervals before the newest state received, so
 * that two states always surround it.
 *
 * It advances with the real time of the frames and is nudged at each state
 * (CORRECTION_GAIN), so network jitter does not make entities speed up and slow
 * down. The interval is the smallest of the last INTERVAL_WINDOW gaps between
 * the ticks of the states received: it follows --state-rate with no setting.
 * One clock serves one game: the ticks start again with each game.
 */
class InterpolationClock {
 public:
  /** @brief Moves the tick shown on by @p elapsed of real time, never beyond
   * the newest tick received plus MAXIMUM_EXTRAPOLATION_TICKS. Nothing before
   * the first state. */
  void advance(engine::Duration elapsed);

  /** @brief Takes the tick of a state received into account; nothing when it
   * is not newer than the newest one (a late copy, or another part of the
   * same state). */
  void observe(std::uint32_t tick);

  /** @returns The tick to show, fractional; nothing before the first state. */
  [[nodiscard]] std::optional<double> renderTick() const;

 private:
  /** @returns The newest tick received minus the delay: where the tick shown
   * would be if every state had arrived on time. */
  [[nodiscard]] double targetTick() const;

  /** @returns The interval between two states, in ticks. */
  [[nodiscard]] double intervalTicks() const;

  /** @brief Remembers the gap between two states received, forgetting the
   * oldest of INTERVAL_WINDOW. */
  void recordGap(std::uint32_t gap);

  /** @returns @p shown with CORRECTION_GAIN of its error to the target taken
   * back, or all of it beyond SNAP_INTERVALS intervals and when the first
   * interval was just measured, which replaces the assumed one. */
  [[nodiscard]] double pulledTowardsTarget(double shown) const;

  std::optional<double> renderTick_;
  std::uint32_t newestTick_{0};
  std::array<std::uint32_t, INTERVAL_WINDOW> gaps_{};
  std::size_t gapCount_{0};
};

}  // namespace rtype::client
