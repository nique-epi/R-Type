#pragma once

#include <cstddef>
#include "BenchPopulation.hpp"
#include "ScriptBudgetBenchConstants.hpp"

namespace rtype::tools {

/** @brief What the command line of the bench asks. */
struct BenchLaunch {
  BenchPopulation population;
  std::size_t ticks{DEFAULT_BENCH_TICKS};
  bool helpRequested{false};
};

}  // namespace rtype::tools
