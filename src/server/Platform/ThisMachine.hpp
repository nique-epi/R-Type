#pragma once

#include <optional>
#include "MachineCounters.hpp"

namespace rtype::server {

/**
 * @brief Reads the counters of the machine the server runs on.
 *
 * Linux gives them in `/proc/stat` and `/proc/meminfo`, where the memory in
 * use is the total less what the kernel says is available. macOS gives them
 * through `host_statistics()`, where the memory in use is the pages of the
 * programs that cannot be purged, the wired ones and the compressed ones.
 * Windows gives them through `GetSystemTimes()`, whose kernel time holds the
 * idle time and which adds up the processors of the group of the calling
 * thread, at most 64, and through `GlobalMemoryStatusEx()`.
 *
 * @returns The counters, or nothing when the system refused one of them.
 */
[[nodiscard]] std::optional<MachineCounters> countersOfThisMachine();

}  // namespace rtype::server
