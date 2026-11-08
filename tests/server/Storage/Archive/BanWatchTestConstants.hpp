#pragma once

#include <chrono>

namespace rtype::server::testing {

/** @brief How long a test waits for the watch to read the bans again: five
 * times BAN_REREAD_INTERVAL, so a slow machine is not taken for a watch that
 * never reads. */
constexpr std::chrono::seconds BAN_WATCH_WAIT{5};

/** @brief How long a test watches for a publication that must not come: two
 * and a half times BAN_REREAD_INTERVAL, so at least two rereads happen. */
constexpr std::chrono::milliseconds TWO_REREADS_AND_A_HALF{2500};

}  // namespace rtype::server::testing
