#pragma once

#include "MachineCounters.hpp"

namespace rtype::server::testing {

/**
 * @brief Two readings of a machine of 8 GB: 1000 units of processor time
 * passed between them, 250 of them idle, so three quarters were busy; 2 GB
 * are in use at the first and 6 GB at the second.
 */
constexpr MachineCounters EARLIER_COUNTERS{.processorTime = 5000,
                                           .idleProcessorTime = 4000,
                                           .usedMemoryBytes = 2'000'000'000,
                                           .totalMemoryBytes = 8'000'000'000};
constexpr MachineCounters LATEST_COUNTERS{.processorTime = 6000,
                                          .idleProcessorTime = 4250,
                                          .usedMemoryBytes = 6'000'000'000,
                                          .totalMemoryBytes = 8'000'000'000};
constexpr double THREE_QUARTERS_BUSY_PERCENT = 75.0;
constexpr double SIX_GIGABYTES = 6'000'000'000.0;
constexpr double EIGHT_GIGABYTES = 8'000'000'000.0;

/** @brief Readings after LATEST_COUNTERS in which every unit that passed was
 * idle, and in which none was. */
constexpr MachineCounters ALL_IDLE_COUNTERS{.processorTime = 7000,
                                            .idleProcessorTime = 5250,
                                            .usedMemoryBytes = 6'000'000'000,
                                            .totalMemoryBytes = 8'000'000'000};
constexpr MachineCounters NONE_IDLE_COUNTERS{.processorTime = 7000,
                                             .idleProcessorTime = 4250,
                                             .usedMemoryBytes = 6'000'000'000,
                                             .totalMemoryBytes = 8'000'000'000};
constexpr double IDLE_PERCENT = 0.0;
constexpr double FLAT_OUT_PERCENT = 100.0;

/** @brief Readings after LATEST_COUNTERS whose idle time went back, and which
 * counted more idle time than time. */
constexpr MachineCounters IDLE_TIME_BACK_COUNTERS{
    .processorTime = 7000,
    .idleProcessorTime = 4249,
    .usedMemoryBytes = 6'000'000'000,
    .totalMemoryBytes = 8'000'000'000};
constexpr MachineCounters MORE_IDLE_THAN_TIME_COUNTERS{
    .processorTime = 7000,
    .idleProcessorTime = 5251,
    .usedMemoryBytes = 6'000'000'000,
    .totalMemoryBytes = 8'000'000'000};

/** @brief Readings of a machine that says it holds no memory, and of one
 * that says it uses one byte more than it holds. */
constexpr MachineCounters NO_MEMORY_COUNTERS{.processorTime = 6000,
                                             .idleProcessorTime = 4250,
                                             .usedMemoryBytes = 0,
                                             .totalMemoryBytes = 0};
constexpr MachineCounters MORE_USED_THAN_HELD_COUNTERS{
    .processorTime = 6000,
    .idleProcessorTime = 4250,
    .usedMemoryBytes = 8'000'000'001,
    .totalMemoryBytes = 8'000'000'000};

/** @brief A measure of the games the load is added to. */
constexpr double GAMES_MEDIAN_TICK_MILLISECONDS = 2.5;

}  // namespace rtype::server::testing
