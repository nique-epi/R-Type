#pragma once

#include "MachineCounterReadings.hpp"
#include "ServerMeasures.hpp"

namespace rtype::server {

/**
 * @brief Adds the load of the machine to the measures of a heartbeat.
 *
 * The memory in use and all the memory come from the latest counters. The
 * share of busy processors is the part of the processor time between the two
 * readings that was not idle, out of network::MAXIMUM_CPU_PERCENT, so it
 * covers the time since the heartbeat before and never passes what the master
 * accepts.
 *
 * @returns @p measures with the memory when the latest counters hold some,
 * and with the share of busy processors when both readings exist and
 * processor time passed between them; a counter that went back, as after a
 * restart of the machine, gives no share.
 */
[[nodiscard]] network::ServerMeasures withMachineLoad(
    network::ServerMeasures measures, const MachineCounterReadings& readings);

}  // namespace rtype::server
