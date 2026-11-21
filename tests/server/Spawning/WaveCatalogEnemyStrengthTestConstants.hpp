#pragma once

#include <string_view>
#include "EnemyStrength.hpp"

namespace rtype::server::testing {

/** @brief The keys of a difficulty that scale its enemies, as the tests write
 * them, and a value of each. */
constexpr std::string_view ENEMY_HEALTH_PERCENT_IN_TESTS =
    "enemy_health_percent";
constexpr std::string_view ENEMY_GUN_INTERVAL_PERCENT_IN_TESTS =
    "enemy_gun_interval_percent";
constexpr std::string_view ONE_HUNDRED_AND_FIFTY_AS_A_VALUE = "150";
constexpr int ONE_HUNDRED_AND_FIFTY_PERCENT = 150;
constexpr std::string_view ZERO_AS_A_VALUE = "0";

/** @brief The strengths of the difficulties of the shipped waves file: easy
 * gives less health and slower guns, hard more health and faster guns, and
 * normal the sheets as they are. */
constexpr game::EnemyStrength EASY_ENEMY_STRENGTH{.healthPercent = 80,
                                                  .gunIntervalPercent = 130};
constexpr game::EnemyStrength NORMAL_ENEMY_STRENGTH{.healthPercent = 100,
                                                    .gunIntervalPercent = 100};
constexpr game::EnemyStrength HARD_ENEMY_STRENGTH{.healthPercent = 120,
                                                  .gunIntervalPercent = 80};

}  // namespace rtype::server::testing
