#pragma once

#include "EnemyStrength.hpp"

namespace rtype::game::testing {

/** @brief The strength of the hard difficulty: 120 % of the health of a
 * sheet, 80 % of the ticks of its gun. */
constexpr EnemyStrength HARD_STRENGTH{.healthPercent = 120,
                                      .gunIntervalPercent = 80};

/** @brief The health of hard for a team of four (FOUR_PLAYERS of
 * BossHealthScalingTestConstants.hpp): 120 % raised by three extra players
 * of 15 %, 145 % of 120 %. */
constexpr int HARD_HEALTH_FOR_FOUR = 174;

}  // namespace rtype::game::testing
