#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::engine {

/** @brief The memory every script of one machine may hold together. */
constexpr std::size_t SCRIPT_MEMORY_LIMIT_BYTES = std::size_t{16} * 1024 * 1024;

/** @brief The time one run or one call of a script may take before it is
 * stopped. */
constexpr std::chrono::milliseconds SCRIPT_CALL_BUDGET{20};

/** @brief The time one step of a thread may take before it is stopped. It is
 * far above what a step needs (a call measured 0.9 µs), and small enough that
 * the step that uses the last of the budget of a tick overruns it little. */
constexpr std::chrono::milliseconds SCRIPT_STEP_BUDGET{2};

/** @brief The time the steps of one tick may take together, under the 16.7 ms
 * of a tick: past it, no step starts before the next tick. */
constexpr std::chrono::milliseconds SCRIPT_TICK_BUDGET{8};

/** @brief How many more loops and calls a script runs past its deadline
 * before it is stopped: a machine that stalls makes a script late for a moment
 * without that script being at fault, and a script that never ends reaches
 * this count within microseconds. */
constexpr std::uint32_t SCRIPT_OVERDUE_SAFEPOINTS = 1024;

/** @brief The share of the memory limit, in percent, past which a new tick
 * collects the garbage at once: otherwise a burst of garbage reaches the limit
 * before the collector does, and fails a script that holds little. */
constexpr std::size_t SCRIPT_COLLECTION_THRESHOLD_PERCENT = 75;

/** @brief The error a script stopped for its budget ends with. */
constexpr const char* SCRIPT_OVER_BUDGET_MESSAGE =
    "the script ran longer than its budget";

/** @brief Globals the sandbox of Luau leaves that let a script reach the
 * globals of another: removed before it is sealed. */
constexpr std::array<std::string_view, 2> UNSAFE_SCRIPT_GLOBALS{"getfenv",
                                                                "setfenv"};

/** @brief The mark that makes Luau show a chunk name as written. */
constexpr std::string_view SCRIPT_CHUNK_NAME_PREFIX = "=";

}  // namespace rtype::engine
