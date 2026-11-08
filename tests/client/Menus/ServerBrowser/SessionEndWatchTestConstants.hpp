#pragma once

#include <chrono>

namespace rtype::client::testing {

/** @brief The map reads the games again 2 s after the player came back from a
 * server: 1999 ms after the end is one millisecond short of it, and a minute
 * is long past it. */
constexpr std::chrono::milliseconds SHORT_OF_THE_RECHECK{1999};
constexpr std::chrono::milliseconds LAST_MILLISECOND{1};
constexpr std::chrono::minutes LONG_PAST_THE_RECHECK{1};

}  // namespace rtype::client::testing
