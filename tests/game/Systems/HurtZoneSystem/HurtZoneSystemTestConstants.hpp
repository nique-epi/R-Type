#pragma once

#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "HurtZone.hpp"

namespace rtype::game::testing {

/** @brief Where the enemy stands, in logical units. */
constexpr float ENEMY_X = 500.0F;
constexpr float ENEMY_Y = 300.0F;

/** @brief A box of 40 by 40 centered 60 units left of the enemy: it spans x
 * 420 to 460 and y 280 to 320. */
constexpr float BOX_OFFSET_X = -60.0F;
constexpr float BOX_SIDE = 40.0F;

/** @brief Ship centers (the ship box is the square of the collision tests):
 * one inside the offset box, one on the enemy itself, one far away. */
constexpr float SHIP_IN_THE_BOX_X = 440.0F;
constexpr float SHIP_ON_THE_ENEMY_X = 500.0F;
constexpr float SHIP_FAR_AWAY_X = 100.0F;

/** @brief The attack of the box under test, and another. */
constexpr std::uint8_t SCYTHE = 1;
constexpr std::uint8_t OTHER_ATTACK = 2;

constexpr ActionKey SCYTHE_STRIKE{.attack = SCYTHE,
                                  .phase = EnemyPhase::Strike};
constexpr ActionKey SCYTHE_SIGNAL{.attack = SCYTHE,
                                  .phase = EnemyPhase::Signal};
constexpr ActionKey OTHER_STRIKE{.attack = OTHER_ATTACK,
                                 .phase = EnemyPhase::Strike};

constexpr HurtBox ALWAYS_ACTIVE_BOX{.offsetX = BOX_OFFSET_X,
                                    .offsetY = 0.0F,
                                    .width = BOX_SIDE,
                                    .height = BOX_SIDE,
                                    .activeIn = std::nullopt};

}  // namespace rtype::game::testing
