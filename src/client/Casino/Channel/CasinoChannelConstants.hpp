#pragma once

#include <chrono>

namespace rtype::client {

/** @brief How long the client waits, after the master answered the state of
 * the room followed, before it asks it again: one read in flight at most. */
constexpr std::chrono::milliseconds CASINO_ROOM_POLL_INTERVAL{500};

}  // namespace rtype::client
