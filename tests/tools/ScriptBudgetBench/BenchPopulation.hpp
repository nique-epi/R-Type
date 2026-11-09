#pragma once

#include <cstddef>
#include "ScriptBudgetBenchConstants.hpp"

namespace rtype::tools {

/** @brief The bodies the bench keeps alive at every tick. */
struct BenchPopulation {
  std::size_t scriptedBodies{DEFAULT_SCRIPTED_BODIES};
  std::size_t bullets{DEFAULT_BULLETS};
};

}  // namespace rtype::tools
