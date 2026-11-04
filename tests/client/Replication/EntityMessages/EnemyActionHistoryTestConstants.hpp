#pragma once

#include <cstdint>
#include "EnemyAction.hpp"

namespace rtype::client::testing {

/** @brief The enemy and the attack of the actions of the tests. */
constexpr std::uint32_t ENEMY_ID = 7;
constexpr std::uint8_t SOME_ATTACK = 1;
constexpr std::uint8_t OTHER_ATTACK = 2;

/** @brief The Signal and the Strike of one attack of that enemy, and the
 * Signal of another attack. */
constexpr network::EnemyAction SIGNAL{
    .entityId = ENEMY_ID,
    .attack = SOME_ATTACK,
    .phase = network::EnemyActionPhase::Signal};
constexpr network::EnemyAction STRIKE{
    .entityId = ENEMY_ID,
    .attack = SOME_ATTACK,
    .phase = network::EnemyActionPhase::Strike};
constexpr network::EnemyAction OTHER_SIGNAL{
    .entityId = ENEMY_ID,
    .attack = OTHER_ATTACK,
    .phase = network::EnemyActionPhase::Signal};

}  // namespace rtype::client::testing
