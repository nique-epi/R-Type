#pragma once

#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"

namespace rtype::game::testing {

/** @brief The network id of the enemy the action tests pilot. */
constexpr std::uint32_t SOME_ENEMY_NETWORK_ID = 12;

/** @brief Two actions an enemy enters: the strike of attack 3 and the signal
 * of attack 4. */
constexpr ActionKey STRIKE_OF_ATTACK_THREE{.attack = 3,
                                           .phase = EnemyPhase::Strike};
constexpr ActionKey SIGNAL_OF_ATTACK_FOUR{.attack = 4,
                                          .phase = EnemyPhase::Signal};

}  // namespace rtype::game::testing
