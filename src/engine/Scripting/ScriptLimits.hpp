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
  /** @brief The time one step of a thread may take. */
  std::chrono::milliseconds stepBudget{SCRIPT_STEP_BUDGET};
  /** @brief The time the steps of one tick may take together. */
  std::chrono::milliseconds tickBudget{SCRIPT_TICK_BUDGET};
};

}  // namespace rtype::engine
