#pragma once

#include <cstdint>
#include "Boss.hpp"

namespace rtype::game::testing {

/** @brief The boss of the damage tests: network id 31, 240 points, its
 * arrival attack 0 and its rage attack 9, enraging below 120 points. */
constexpr std::uint32_t BOSS_ID = 31;
constexpr int BOSS_FULL_HEALTH = 240;
constexpr int BOSS_HEALTH_AFTER_ONE_HIT = 239;
constexpr std::uint8_t BOSS_ARRIVAL_ATTACK = 0;
constexpr std::uint8_t BOSS_RAGE_ATTACK = 9;
constexpr int BOSS_RAGE_BELOW_POINTS = 120;
constexpr Boss CALM_BOSS{.rageBelowPoints = BOSS_RAGE_BELOW_POINTS,
                         .rageAttack = BOSS_RAGE_ATTACK,
                         .arrivalAttack = BOSS_ARRIVAL_ATTACK};

}  // namespace rtype::game::testing
