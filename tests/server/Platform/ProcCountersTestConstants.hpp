#pragma once

#include <cstdint>
#include <string_view>

/**
 * @brief The `cpu` line of a machine of two processors, the first of
 * `/proc/stat`: user 100, nice 5, system 40, idle 800, iowait 20, irq 3,
 * softirq 2, steal 30, then the guest times, which the user time already
 * counts. The eight first make 1000 ticks, 820 of them idle or waiting for
 * the disk.
 */
constexpr std::string_view TWO_PROCESSORS_TEXT =
    "cpu  100 5 40 800 20 3 2 30 7 1\n"
    "cpu0 50 2 20 400 10 2 1 15 4 1\n"
    "cpu1 50 3 20 400 10 1 1 15 3 0\n"
    "intr 12345 0 0\n";
constexpr std::uint64_t TWO_PROCESSORS_TIME = 1000;
constexpr std::uint64_t TWO_PROCESSORS_IDLE_TIME = 820;

/** @brief The `cpu` line of a kernel older than 2.5.41: four times, 945
 * ticks, 800 of them idle. */
constexpr std::string_view FOUR_FIELDS_PROCESSORS_TEXT = "cpu 100 5 40 800\n";
constexpr std::uint64_t FOUR_FIELDS_TIME = 945;
constexpr std::uint64_t FOUR_FIELDS_IDLE_TIME = 800;

/** @brief Texts of `/proc/stat` with no line that adds up the processors, and
 * with one of three times. */
constexpr std::string_view NO_SUM_PROCESSORS_TEXT = "cpu0 50 2 20 400\n";
constexpr std::string_view THREE_FIELDS_PROCESSORS_TEXT = "cpu 100 5 40\n";

/** @brief A machine of 8 GiB of which 5 GiB are available: 3 GiB in use.
 * `MemFree` is lower than `MemAvailable`, as on a machine with a cache. */
constexpr std::string_view EIGHT_GIBIBYTES_TEXT =
    "MemTotal:        8388608 kB\n"
    "MemFree:         1048576 kB\n"
    "MemAvailable:    5242880 kB\n"
    "Buffers:          204800 kB\n";
constexpr std::uint64_t EIGHT_GIBIBYTES = 8'589'934'592;
constexpr std::uint64_t THREE_GIBIBYTES = 3'221'225'472;

/** @brief Texts of `/proc/meminfo` without the available memory, with more
 * of it than the machine holds, and with a total that is no number. */
constexpr std::string_view NO_AVAILABLE_MEMORY_TEXT =
    "MemTotal:        8388608 kB\nMemFree:         1048576 kB\n";
constexpr std::string_view MORE_AVAILABLE_THAN_TOTAL_TEXT =
    "MemTotal:        8388608 kB\nMemAvailable:    8388609 kB\n";
constexpr std::string_view WORDY_TOTAL_MEMORY_TEXT =
    "MemTotal:        plenty kB\nMemAvailable:    5242880 kB\n";
