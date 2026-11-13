#pragma once

#include <optional>
#include "MachineCounters.hpp"
#include "ProcTexts.hpp"

namespace rtype::server {

/**
 * @brief Reads the counters of a Linux machine in the text of its `/proc`
 * files, as proc_stat(5) and proc_meminfo(5) describe them.
 *
 * The processor time is the sum of the first PROCESSOR_TIME_FIELD_COUNT
 * fields of the `cpu` line, in clock ticks; the guest times that follow are
 * already counted in the user time. Its idle part is the idle time and the
 * time spent waiting for the disk. The memory in use is `MemTotal` less
 * `MemAvailable`, both in kibibytes.
 *
 * @returns The counters, or nothing when the `cpu` line holds fewer than
 * FEWEST_PROCESSOR_FIELDS numbers, when a memory line is missing or holds no
 * number, or when more memory is available than the machine holds.
 */
[[nodiscard]] std::optional<MachineCounters> countersIn(const ProcTexts& texts);

}  // namespace rtype::server
