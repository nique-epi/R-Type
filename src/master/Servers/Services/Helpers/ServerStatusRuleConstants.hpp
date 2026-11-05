#pragma once

#include <chrono>
#include "DurationConversion.hpp"
#include "MasterMessageConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::master {

/**
 * @brief How many heartbeats a server may miss, and the silence after which
 * it is offline: 3 of network::MASTER_HEARTBEAT_INTERVAL (5 s), 15 s.
 */
constexpr int MISSED_HEARTBEATS_BEFORE_OFFLINE = 3;
constexpr std::chrono::milliseconds OFFLINE_SILENCE =
    MISSED_HEARTBEATS_BEFORE_OFFLINE * network::MASTER_HEARTBEAT_INTERVAL;

/**
 * @brief The bounds a server is degraded at: the 95th percentile of its ticks
 * at one tick of the simulation (engine::SIMULATION_TICK_DURATION, 16.67 ms)
 * or more, or 5 % of the datagrams of its players lost or more. Starting
 * values, to set again once the measurement campaigns are done.
 */
constexpr double DEGRADED_TICK_P95_MILLISECONDS =
    engine::inMilliseconds(engine::SIMULATION_TICK_DURATION);
constexpr double DEGRADED_LOSS_PERCENT = 5.0;

}  // namespace rtype::master
