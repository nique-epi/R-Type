#pragma once

#include <chrono>
#include "HeartbeatTimes.hpp"

namespace rtype::server {

/**
 * @brief The waits of MasterLink between two registrations the master did not
 * take: exponential backoff with full jitter, as Marc Brooker describes it in
 * "Exponential Backoff And Jitter" (AWS Architecture Blog). The ceiling of the
 * wait starts at HeartbeatTimes::retry, or at HeartbeatTimes::maximumRetry
 * when that is shorter, and doubles after each wait, up to
 * HeartbeatTimes::maximumRetry; each wait is drawn between zero and that
 * ceiling, so servers that lost the same master do not all call it again in
 * the same second once it is back.
 */
class RetryBackoff {
 public:
  explicit RetryBackoff(HeartbeatTimes times);

  /**
   * @returns The wait before the next registration: the ceiling times
   * @p draw, a number from 0 to 1. The ceiling of the wait after it doubles,
   * up to the maximum.
   */
  [[nodiscard]] std::chrono::milliseconds nextWait(double draw);

  /** @brief Starts again from the first ceiling, HeartbeatTimes::retry or the
   * maximum when that is shorter: the master took the registration, so the
   * next failure is a new one. */
  void reset();

 private:
  std::chrono::milliseconds first_;
  std::chrono::milliseconds maximum_;
  std::chrono::milliseconds ceiling_;
};

}  // namespace rtype::server
