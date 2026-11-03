#pragma once

#include <vector>
#include "TickDurationSummary.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The durations of the ticks of one report: at most TICKS_PER_REPORT, in
 * a buffer that reserves its room when it is built, so recording a tick
 * allocates nothing.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class TickDurations {
 public:
  TickDurations();

  /** @brief Keeps @p duration; does nothing once the buffer is full. */
  void record(engine::Duration duration);

  /** @returns Whether TICKS_PER_REPORT durations are kept. */
  [[nodiscard]] bool isFull() const;

  /**
   * @returns What the durations kept say, then forgets them. It sorts them, so
   * it is called once per report, never per tick.
   */
  [[nodiscard]] TickDurationSummary takeSummary();

 private:
  std::vector<engine::Duration> samples_;
};

}  // namespace rtype::server
