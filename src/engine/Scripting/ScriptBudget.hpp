#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "ScriptLimits.hpp"

namespace rtype::engine {

/** @brief What a script machine spends of its limits: the memory its scripts
 * hold, the time the call in progress may run until, and the time the steps
 * of this tick took. */
struct ScriptBudget {
  ScriptLimits limits;
  std::size_t memoryUsed{0};
  std::chrono::steady_clock::time_point deadline;
  bool callInProgress{false};
  /** @brief The loops and calls the call in progress ran past its deadline. */
  std::uint32_t overdueSafepoints{0};
  std::chrono::steady_clock::duration tickSpent{};
};

}  // namespace rtype::engine
