#pragma once

#include <chrono>
#include <cstddef>
#include "ScriptingConstants.hpp"

namespace rtype::engine {

/** @brief What a script machine allows its scripts. */
struct ScriptLimits {
  /** @brief The memory all its scripts may hold together, in bytes. */
  std::size_t memoryBytes{SCRIPT_MEMORY_LIMIT_BYTES};
  /** @brief The time one run or one call may take. */
  std::chrono::milliseconds callBudget{SCRIPT_CALL_BUDGET};
};

}  // namespace rtype::engine
