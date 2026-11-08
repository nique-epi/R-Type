#include "RetryBackoff.hpp"
#include <algorithm>
#include <chrono>
#include <ratio>
#include "HeartbeatTimes.hpp"
#include "ServerMasterConstants.hpp"

namespace rtype::server {

RetryBackoff::RetryBackoff(HeartbeatTimes times)
    : first_(times.retry),
      maximum_(times.maximumRetry),
      ceiling_(std::min(times.retry, times.maximumRetry)) {}

std::chrono::milliseconds RetryBackoff::nextWait(double draw) {
  const std::chrono::milliseconds ceiling = ceiling_;
  ceiling_ = std::min(ceiling_ * RETRY_GROWTH, maximum_);
  return std::chrono::round<std::chrono::milliseconds>(
      std::chrono::duration<double, std::milli>{ceiling} * draw);
}

void RetryBackoff::reset() { ceiling_ = std::min(first_, maximum_); }

}  // namespace rtype::server
