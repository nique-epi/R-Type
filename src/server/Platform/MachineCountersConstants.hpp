#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server {

/** @brief The files a Linux machine counts its processors and its memory in. */
constexpr std::string_view PROCESSORS_FILE = "/proc/stat";
constexpr std::string_view MEMORY_FILE = "/proc/meminfo";

/** @brief How the line of `/proc/stat` that adds up every processor starts. */
constexpr std::string_view ALL_PROCESSORS_LINE_START = "cpu ";

/**
 * @brief The fields of that line: the first PROCESSOR_TIME_FIELD_COUNT are
 * user, nice, system, idle, iowait, irq, softirq and steal, and a kernel
 * older than 2.5.41 writes FEWEST_PROCESSOR_FIELDS of them only.
 */
constexpr std::size_t PROCESSOR_TIME_FIELD_COUNT = 8;
constexpr std::size_t FEWEST_PROCESSOR_FIELDS = 4;
constexpr std::size_t IDLE_FIELD = 3;
constexpr std::size_t DISK_WAIT_FIELD = 4;

/** @brief How the lines of `/proc/meminfo` that hold all the memory and the
 * memory a new program may take start. */
constexpr std::string_view TOTAL_MEMORY_LINE_START = "MemTotal:";
constexpr std::string_view AVAILABLE_MEMORY_LINE_START = "MemAvailable:";

/** @brief The characters between two fields of a line of those files. */
constexpr std::string_view PROC_FIELD_BLANKS = " \t";

/** @brief The bytes of the unit `/proc/meminfo` counts in. */
constexpr std::uint64_t BYTES_PER_KIBIBYTE = 1024;

/** @brief The setting macOS keeps all the memory of the machine in. */
constexpr const char* MACOS_TOTAL_MEMORY_SETTING = "hw.memsize";

/** @brief The bits of the low half of a Windows FILETIME. */
constexpr unsigned FILETIME_LOW_PART_BITS = 32;

}  // namespace rtype::server
