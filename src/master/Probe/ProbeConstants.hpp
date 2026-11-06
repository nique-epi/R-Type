#pragma once

#include <chrono>
#include <cstddef>
#include "MasterMessageConstants.hpp"

namespace rtype::master {

/**
 * @brief The least time between two probes to one address and port: the least
 * time between two heartbeats of a game server, so a server that beats as fast
 * as it may still gets every probe, and a destination never gets probes
 * faster, whoever registers it.
 */
constexpr std::chrono::milliseconds PROBE_MINIMUM_GAP =
    network::MINIMUM_NOTICE_GAP;

/**
 * @brief How many destinations ProbeThrottle remembers before it forgets
 * those whose gap passed.
 */
constexpr std::size_t MAXIMUM_THROTTLED_DESTINATIONS = 4'096;

}  // namespace rtype::master
