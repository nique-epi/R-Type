#include "MachineLoad.hpp"
#include <cstdint>
#include <optional>
#include "MachineCounterReadings.hpp"
#include "MachineCounters.hpp"
#include "MasterMessageConstants.hpp"
#include "ServerMeasures.hpp"

namespace rtype::server {

namespace {

/** @returns The share of the processor time between @p earlier and @p latest
 * that was not idle, 0 to 100; nothing when no time passed or a counter went
 * back. */
std::optional<double> busyPercentBetween(const MachineCounters& earlier,
                                         const MachineCounters& latest) {
  if (latest.processorTime <= earlier.processorTime ||
      latest.idleProcessorTime < earlier.idleProcessorTime) {
    return std::nullopt;
  }
  const std::uint64_t passed = latest.processorTime - earlier.processorTime;
  const std::uint64_t idle =
      latest.idleProcessorTime - earlier.idleProcessorTime;
  if (idle > passed) {
    return std::nullopt;
  }
  return network::MAXIMUM_CPU_PERCENT * static_cast<double>(passed - idle) /
         static_cast<double>(passed);
}

}  // namespace

network::ServerMeasures withMachineLoad(
    network::ServerMeasures measures, const MachineCounterReadings& readings) {
  if (!readings.latest.has_value()) {
    return measures;
  }
  const MachineCounters& latest = *readings.latest;
  if (latest.totalMemoryBytes > 0 &&
      latest.usedMemoryBytes <= latest.totalMemoryBytes) {
    measures.usedMemoryBytes = static_cast<double>(latest.usedMemoryBytes);
    measures.totalMemoryBytes = static_cast<double>(latest.totalMemoryBytes);
  }
  if (readings.earlier.has_value()) {
    measures.cpuPercent = busyPercentBetween(*readings.earlier, latest);
  }
  return measures;
}

}  // namespace rtype::server
