#pragma once

#include <chrono>

namespace rtype::master {

/**
 * @brief How often the master refreshes the status of every server. A server
 * that stops beating is offline from OFFLINE_SILENCE after its last heartbeat,
 * and the refresh after that writes it, at most one interval later.
 */
constexpr std::chrono::milliseconds STATUS_REFRESH_INTERVAL{5'000};

}  // namespace rtype::master
