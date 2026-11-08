#pragma once

#include <chrono>

namespace rtype::server::testing {

/** @brief How long a test waits for the watch to read the bans again: five
 * times BAN_REREAD_INTERVAL, so a slow machine is not taken for a watch that
 * never reads. */
constexpr std::chrono::seconds BAN_WATCH_WAIT{5};

}  // namespace rtype::server::testing
