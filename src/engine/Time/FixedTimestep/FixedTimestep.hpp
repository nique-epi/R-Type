#pragma once

#include <cstddef>
#include "IClock.hpp"

namespace rtype::engine {

/**
 * @brief Converts elapsed real time into a count of fixed simulation ticks.
 *
 * The clock must outlive the timestep. Time that does not fill a whole tick is
 * kept for the next call. After a stall longer than MAXIMUM_TICKS_PER_ADVANCE
 * ticks, the excess is dropped instead of being caught up.
 */
class FixedTimestep {
 public:
  FixedTimestep(const IClock& clock, Duration tickDuration);

  /** @return number of ticks to simulate since the previous call. */
  std::size_t consumeTicks();

  /**
   * @return the time of the clock at which the next tick is due, from the
   * clock reading of the previous consumeTicks() call, or of the construction:
   * later than that reading by more than zero and at most one tick duration.
   */
  [[nodiscard]] Duration nextTickTime() const;

 private:
  const IClock* clock_;
  Duration tickDuration_;
  Duration previousTime_;
  Duration accumulated_{};
};

}  // namespace rtype::engine
