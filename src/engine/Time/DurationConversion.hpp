#pragma once

#include <chrono>
#include <cstddef>
#include <ratio>
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @returns @p duration in milliseconds, with its fraction: 1.5 for 1500
 * microseconds.
 */
constexpr double inMilliseconds(Duration duration) {
  return std::chrono::duration<double, std::milli>(duration).count();
}

/**
 * @returns @p total shared equally between @p count parts, zero when there is
 * none.
 */
constexpr Duration averageOf(Duration total, std::size_t count) {
  if (count == 0) {
    return Duration::zero();
  }
  return total / static_cast<Duration::rep>(count);
}

}  // namespace rtype::engine
