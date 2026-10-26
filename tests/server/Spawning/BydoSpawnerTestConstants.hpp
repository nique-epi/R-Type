#pragma once

#include <cstddef>
#include <cstdint>
#include "BydoSpawnerConstants.hpp"
#include "TicksToReach.hpp"

namespace rtype::server::testing {

/**
 * @brief The ticks it takes for a whole BYDO_SPAWN_INTERVAL to pass, the last
 * of them the first to reach the interval.
 */
constexpr std::size_t TICKS_PER_BYDO_INTERVAL =
    engine::testing::ticksToReach(BYDO_SPAWN_INTERVAL);

/**
 * @brief The seed the spawner tests start from, and another one, which gives
 * other heights.
 */
constexpr std::uint32_t SPAWNER_SEED = 42;
constexpr std::uint32_t OTHER_SPAWNER_SEED = 43;

/**
 * @brief How many intervals the tests of the heights let pass: enough Bydos
 * for two seeds to give different heights.
 */
constexpr std::size_t SOME_BYDO_INTERVALS = 20;

}  // namespace rtype::server::testing
