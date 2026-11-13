// <mach/mach.h> is the header the Mach API is used through;
// misc-include-cleaner asks for the internal header of each of its names
// instead. NOLINTBEGIN(misc-include-cleaner)
#include <mach/mach.h>
#include <sys/sysctl.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "MachineCounters.hpp"
#include "MachineCountersConstants.hpp"
#include "ThisMachine.hpp"

namespace rtype::server {

namespace {

/** @returns The ticks each state of the processors was in since the machine
 * started; nothing when the kernel refused them. */
std::optional<host_cpu_load_info_data_t> processorTicksOf(host_t host) {
  host_cpu_load_info_data_t ticks{};
  mach_msg_type_number_t size = HOST_CPU_LOAD_INFO_COUNT;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto* answer = reinterpret_cast<host_info_t>(&ticks);
  if (host_statistics(host, HOST_CPU_LOAD_INFO, answer, &size) !=
      KERN_SUCCESS) {
    return std::nullopt;
  }
  return ticks;
}

/** @returns The bytes of the pages in use; nothing when the kernel refused
 * their counts or the size of a page. */
std::optional<std::uint64_t> usedMemoryBytesOf(host_t host) {
  vm_statistics64_data_t pages{};
  mach_msg_type_number_t size = HOST_VM_INFO64_COUNT;
  vm_size_t pageSize = 0;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto* answer = reinterpret_cast<host_info64_t>(&pages);
  if (host_statistics64(host, HOST_VM_INFO64, answer, &size) != KERN_SUCCESS ||
      host_page_size(host, &pageSize) != KERN_SUCCESS ||
      pages.internal_page_count < pages.purgeable_count) {
    return std::nullopt;
  }
  const std::uint64_t used = std::uint64_t{pages.internal_page_count} -
                             pages.purgeable_count + pages.wire_count +
                             pages.compressor_page_count;
  return used * pageSize;
}

/** @returns All the memory of the machine; nothing when the kernel refused
 * it. */
std::optional<std::uint64_t> totalMemoryBytes() {
  std::uint64_t total = 0;
  std::size_t size = sizeof(total);
  if (sysctlbyname(MACOS_TOTAL_MEMORY_SETTING, &total, &size, nullptr, 0) !=
      0) {
    return std::nullopt;
  }
  return total;
}

}  // namespace

std::optional<MachineCounters> countersOfThisMachine() {
  const host_t host = mach_host_self();
  const std::optional<host_cpu_load_info_data_t> ticks = processorTicksOf(host);
  const std::optional<std::uint64_t> used = usedMemoryBytesOf(host);
  const std::optional<std::uint64_t> total = totalMemoryBytes();
  static_cast<void>(mach_port_deallocate(mach_task_self(), host));
  if (!ticks.has_value() || !used.has_value() || !total.has_value()) {
    return std::nullopt;
  }
  const std::uint64_t idle = ticks->cpu_ticks[CPU_STATE_IDLE];
  const std::uint64_t busy = std::uint64_t{ticks->cpu_ticks[CPU_STATE_USER]} +
                             ticks->cpu_ticks[CPU_STATE_SYSTEM] +
                             ticks->cpu_ticks[CPU_STATE_NICE];
  return MachineCounters{.processorTime = busy + idle,
                         .idleProcessorTime = idle,
                         .usedMemoryBytes = *used,
                         .totalMemoryBytes = *total};
}

}  // namespace rtype::server
// NOLINTEND(misc-include-cleaner)
