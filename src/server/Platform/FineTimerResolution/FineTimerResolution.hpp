#pragma once

namespace rtype::server {

/**
 * @brief While it lives, asks Windows to wake sleeping threads within
 * FINE_TIMER_RESOLUTION_MILLISECONDS of their deadline, instead of on the
 * system's coarser default timer tick. Does nothing on other systems.
 *
 * The request only covers this process, and is withdrawn when the object is
 * destroyed.
 */
class FineTimerResolution {
 public:
  FineTimerResolution();
  // NOLINTNEXTLINE(performance-trivially-destructible)
  ~FineTimerResolution();

  FineTimerResolution(const FineTimerResolution&) = delete;
  FineTimerResolution& operator=(const FineTimerResolution&) = delete;
  FineTimerResolution(FineTimerResolution&&) = delete;
  FineTimerResolution& operator=(FineTimerResolution&&) = delete;
};

}  // namespace rtype::server
