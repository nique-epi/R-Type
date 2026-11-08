#pragma once

#include <cstdint>
#include "ActionKey.hpp"
#include "Boss.hpp"
#include "EnemyPhase.hpp"

namespace rtype::game::testing {

/** @brief The boss of the phase tests: network id 41, 240 points, enraging
 * below 120 into its attack 9; its arrival is attack 0. */
constexpr std::uint32_t PHASED_BOSS_ID = 41;
constexpr int PHASED_BOSS_FULL_HEALTH = 240;
constexpr int PHASED_BOSS_RAGE_BELOW = 120;
constexpr int PHASED_BOSS_BELOW_ITS_RAGE = 119;
constexpr int PHASED_BOSS_AT_ITS_RAGE = 120;
constexpr std::uint8_t PHASED_BOSS_RAGE_ATTACK = 9;
constexpr std::uint8_t PHASED_BOSS_ARRIVAL_ATTACK = 0;
constexpr Boss ARRIVING_BOSS{.rageBelowPoints = PHASED_BOSS_RAGE_BELOW,
                             .rageAttack = PHASED_BOSS_RAGE_ATTACK,
                             .arrivalAttack = PHASED_BOSS_ARRIVAL_ATTACK};
constexpr Boss FIGHTING_BOSS{.rageBelowPoints = PHASED_BOSS_RAGE_BELOW,
                             .rageAttack = PHASED_BOSS_RAGE_ATTACK,
                             .arrivalAttack = PHASED_BOSS_ARRIVAL_ATTACK,
                             .fighting = true};

/** @brief The slide of its arrival, and the count of actions a boss has
 * ended once its arrival is over. */
constexpr ActionKey PHASED_BOSS_SLIDE{.attack = PHASED_BOSS_ARRIVAL_ATTACK,
                                      .phase = EnemyPhase::Signal};
constexpr std::uint32_t ARRIVAL_ENDED = 1;

}  // namespace rtype::game::testing
