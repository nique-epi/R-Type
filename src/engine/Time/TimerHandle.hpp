#pragma once

#include <cstdint>

namespace rtype::engine {

/** @brief Identifies one timer of a TimerScheduler; never reused. */
struct TimerHandle {
  std::uint64_t identifier;

  bool operator==(const TimerHandle&) const = default;
};

}  // namespace rtype::engine
