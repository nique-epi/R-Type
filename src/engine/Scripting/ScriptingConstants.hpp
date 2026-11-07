#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::engine {

/** @brief The memory every script of one machine may hold together. */
constexpr std::size_t SCRIPT_MEMORY_LIMIT_BYTES = std::size_t{16} * 1024 * 1024;

/** @brief The time one run or one call of a script may take before it is
 * stopped. */
constexpr std::chrono::milliseconds SCRIPT_CALL_BUDGET{20};

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
