#include "MeasureMilliseconds.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include "DurationConversion.hpp"

namespace rtype::server {

std::optional<double> millisecondsOf(std::optional<std::uint32_t> measure) {
  if (!measure.has_value()) {
    return std::nullopt;
  }
  return engine::inMilliseconds(std::chrono::microseconds{*measure});
}

}  // namespace rtype::server
