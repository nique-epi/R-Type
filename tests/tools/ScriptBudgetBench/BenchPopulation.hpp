#pragma once

#include <cstddef>
#include "ScriptBudgetBenchConstants.hpp"

namespace rtype::tools {

/** @brief The bodies the bench keeps alive at every tick. */
struct BenchPopulation {
  std::size_t scriptedBodies{DEFAULT_SCRIPTED_BODIES};
  std::size_t bullets{DEFAULT_BULLETS};
  /** @brief Whether the bodies run a script that waits, resumed every tick,
   * instead of one that returns. */
  bool waiting{false};
};

}  // namespace rtype::tools
