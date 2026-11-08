#pragma once

#include <chrono>
#include <cstddef>
#include "ScriptLimits.hpp"

namespace rtype::engine {

/** @brief What a script machine spends of its limits: the memory its scripts
 * hold, and the time the call in progress may run until. */
struct ScriptBudget {
  ScriptLimits limits;
  std::size_t memoryUsed{0};
  std::chrono::steady_clock::time_point deadline;
  bool callInProgress{false};
};

}  // namespace rtype::engine
