#include "EpochSeconds.hpp"
#include <chrono>
#include <cstdint>

namespace rtype::master {

std::int64_t epochSecondsOf(std::chrono::sys_seconds date) {
  return static_cast<std::int64_t>(date.time_since_epoch().count());
}

std::chrono::sys_seconds dateOfEpochSeconds(std::int64_t seconds) {
  return std::chrono::sys_seconds{std::chrono::seconds{seconds}};
}

}  // namespace rtype::master
