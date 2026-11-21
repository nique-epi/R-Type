#pragma once

#include <cstddef>
#include "EnemyStrength.hpp"

namespace rtype::game::testing {

/** @brief A strength that gives enemies half their health more, and the
 * health of a Drifter (3 points) at it: 4.5, rounded down to 4. */
constexpr EnemyStrength HALF_AGAIN_THE_HEALTH{.healthPercent = 150,
                                              .gunIntervalPercent = 100};
constexpr int DRIFTER_HEALTH_HALF_AGAIN = 4;

/** @brief A strength that halves the ticks of the guns of enemies, and the
 * gun of a Drifter (90 ticks between shots and before the first) at it. */
constexpr EnemyStrength HALF_THE_GUN_INTERVAL{.healthPercent = 100,
                                              .gunIntervalPercent = 50};
constexpr std::size_t DRIFTER_GUN_TICKS_HALVED = 45;

/** @brief A strength that would leave a gun of 90 ticks under one tick
 * between two shots: 1 % of 90 is 0.9. */
constexpr EnemyStrength ONE_PERCENT_OF_THE_GUN_INTERVAL{
    .healthPercent = 100, .gunIntervalPercent = 1};
constexpr std::size_t FASTEST_GUN_TICKS = 1;

/** @brief A strength stronger on every count, for the bodies it must leave
 * as their sheets give them: a boss, the ship of a player, an enemy already
 * in the world. */
constexpr EnemyStrength STRONGER_ON_EVERY_COUNT{.healthPercent = 200,
                                                .gunIntervalPercent = 50};

}  // namespace rtype::game::testing
