#pragma once

#include <chrono>
#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The network id of the enemy the action tests pilot. */
constexpr std::uint32_t SOME_ENEMY_NETWORK_ID = 12;

/** @brief Two actions an enemy enters: the strike of attack 3 and the signal
 * of attack 4. */
constexpr ActionKey STRIKE_OF_ATTACK_THREE{.attack = 3,
                                           .phase = EnemyPhase::Strike};
constexpr ActionKey SIGNAL_OF_ATTACK_FOUR{.attack = 4,
                                          .phase = EnemyPhase::Signal};

/** @brief How many ticks the clock tests run: three, so a clock that counts
 * one tick only, or none, is told from one that counts each. */
constexpr int CLOCK_TICKS = 3;

/** @brief How long an enemy has been in its phase or idle before a test
 * changes it: longer than any tick, so a clock left running shows. */
constexpr engine::Duration A_SECOND_IN_ITS_PHASE = std::chrono::seconds(1);

}  // namespace rtype::game::testing
