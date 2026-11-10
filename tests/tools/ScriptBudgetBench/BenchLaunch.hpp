#pragma once

#include <cstddef>
#include "BenchPopulation.hpp"
#include "CollisionSystemConstants.hpp"
#include "ScriptBudgetBenchConstants.hpp"

namespace rtype::tools {

/** @brief What the command line of the bench asks. */
struct BenchLaunch {
  BenchPopulation population;
  std::size_t ticks{DEFAULT_BENCH_TICKS};
  /** @brief The side of a cell of the collision grid, in logical units. */
  float cellSide{game::COLLISION_CELL_SIDE};
  bool helpRequested{false};
};

}  // namespace rtype::tools
