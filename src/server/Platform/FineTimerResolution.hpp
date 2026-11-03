#pragma once

namespace rtype::server {

/**
 * @brief While it lives, asks Windows to wake sleeping threads within
 * FINE_TIMER_RESOLUTION_MILLISECONDS of their deadline, instead of on the
 * system's coarser default timer tick. Does nothing on other systems.
 *
 * The request only covers this process. A granted request is withdrawn when
 * the object is destroyed, with the period it was granted with, which Windows
 * only refuses when out of range, so that result is not checked.
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

  /**
   * @returns false when Windows refused the resolution, so sleeping threads
   * still wake on its default timer tick; true otherwise, and always on other
   * systems.
   */
  [[nodiscard]] bool isGranted() const;

 private:
  bool granted_{true};
};

}  // namespace rtype::server
