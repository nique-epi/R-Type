#include "ThrottledLog.hpp"
#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>

namespace rtype::logging {

ThrottledLog::ThrottledLog(std::string module,
                           std::chrono::nanoseconds interval)
    : logger_(std::move(module)), interval_(interval) {}

std::optional<std::size_t> ThrottledLog::countEvent(
    std::chrono::nanoseconds now) {
  ++eventsSincePreviousLine_;
  if (previousLineTime_.has_value() && now - *previousLineTime_ < interval_) {
    return std::nullopt;
  }
  previousLineTime_ = now;
  return std::exchange(eventsSincePreviousLine_, std::size_t{0});
}

}  // namespace rtype::logging
