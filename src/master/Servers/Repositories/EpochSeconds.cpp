#include "EpochSeconds.hpp"
#include <chrono>
#include <cstdint>

namespace rtype::master {

std::int64_t epochSecondsOf(std::chrono::sys_seconds date) {
  return static_cast<std::int64_t>(date.time_since_epoch().count());
}

}  // namespace rtype::master
