#include <windows.h>
#include <cstdint>
#include <optional>
#include "MachineCounters.hpp"
#include "MachineCountersConstants.hpp"
#include "ThisMachine.hpp"

namespace rtype::server {

namespace {

/** @returns The count of 100 ns intervals @p time holds in two halves. */
std::uint64_t intervalsOf(const FILETIME& time) {
  return (std::uint64_t{time.dwHighDateTime} << FILETIME_LOW_PART_BITS) |
         std::uint64_t{time.dwLowDateTime};
}

}  // namespace

std::optional<MachineCounters> countersOfThisMachine() {
  FILETIME idle{};
  FILETIME kernel{};
  FILETIME user{};
  MEMORYSTATUSEX memory{};
  memory.dwLength = sizeof(memory);
  if (GetSystemTimes(&idle, &kernel, &user) == 0 ||
      GlobalMemoryStatusEx(&memory) == 0 ||
      memory.ullAvailPhys > memory.ullTotalPhys) {
    return std::nullopt;
  }
  return MachineCounters{
      .processorTime = intervalsOf(kernel) + intervalsOf(user),
      .idleProcessorTime = intervalsOf(idle),
      .usedMemoryBytes = memory.ullTotalPhys - memory.ullAvailPhys,
      .totalMemoryBytes = memory.ullTotalPhys};
}

}  // namespace rtype::server
