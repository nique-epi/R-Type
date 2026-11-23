#pragma once

#include "EnemyStrength.hpp"

namespace rtype::game::testing {

/** @brief The strength of the hard difficulty: 120 % of the health of a
 * sheet, 80 % of the ticks of its gun. */
constexpr EnemyStrength HARD_STRENGTH{.healthPercent = 120,
                                      .gunIntervalPercent = 80};

/** @brief The health of hard for a team of four (FOUR_PLAYERS of
 * BossHealthScalingTestConstants.hpp): 120 % raised by three extra players
 * of 40 %, 220 % of 120 %. */
constexpr int HARD_HEALTH_FOR_FOUR = 264;

/** @brief The gun interval of hard for a team of four: 80 % lowered by three
 * extra players of 10 %, 70 % of 80 %. */
constexpr int HARD_GUN_INTERVAL_FOR_FOUR = 56;

/** @brief A threat of none, of half the most and of the most, and what hard
 * gives then: the health raised 50 % at the most, 150 % of 120 %, and 25 %
 * at half, 125 % of 120 %; the gun interval lowered 23 % at the most, 77 %
 * of 80 %. */
constexpr int NO_THREAT = 0;
constexpr int HALF_THE_MOST_THREAT = 50;
constexpr int THE_MOST_THREAT = 100;
constexpr int HARD_HEALTH_AT_THE_MOST_THREAT = 180;
constexpr int HARD_HEALTH_AT_HALF_THE_MOST_THREAT = 150;
constexpr int HARD_GUN_INTERVAL_AT_THE_MOST_THREAT = 61;

}  // namespace rtype::game::testing
