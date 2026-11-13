#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @brief What a system counts of its whole machine, every program included,
 * at one moment. The load of the processors between two moments is read from
 * two of them: the time that passed and was not idle.
 */
struct MachineCounters {
  /** @brief The time every processor spent since the machine started, idle
   * time included, and the idle part of it, both in the unit of the system. */
  std::uint64_t processorTime{0};
  std::uint64_t idleProcessorTime{0};
  /** @brief The memory in use and all the memory of the machine. */
  std::uint64_t usedMemoryBytes{0};
  std::uint64_t totalMemoryBytes{0};

  bool operator==(const MachineCounters&) const = default;
};

}  // namespace rtype::server
