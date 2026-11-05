#pragma once

#include "CollisionBox.hpp"
#include "EntityType.hpp"
#include "Muzzle.hpp"

namespace rtype::game::testing {

/** @brief A ship and a Bydo somewhere in the play area, with the boxes of
 * their sprites: 128 by 64 and 96 by 96. */
constexpr Muzzle SOME_SHIP{.center = {.x = 500.0F, .y = 300.0F},
                           .box = {.width = 128.0F, .height = 64.0F},
                           .side = EntityType::Player};
constexpr Muzzle SOME_BYDO{.center = {.x = 1000.0F, .y = 300.0F},
                           .box = {.width = 96.0F, .height = 96.0F},
                           .side = EntityType::Enemy};

/** @brief The boxes of a missile of a player, 64 by 24, and of an enemy, 32 by
 * 32. */
constexpr CollisionBox PLAYER_MISSILE_BOX{.width = 64.0F, .height = 24.0F};
constexpr CollisionBox ENEMY_MISSILE_BOX{.width = 32.0F, .height = 32.0F};

/**
 * @brief Where the missile of SOME_SHIP starts: the front of the ship is half
 * of its 128 units past its center, at 564, and the center of the missile is
 * half of its 64 units further, at 596.
 */
constexpr float SHIP_MISSILE_START_X = 596.0F;
/**
 * @brief Where the missile of SOME_BYDO starts: the left side of the Bydo is
 * half of its 96 units before its center, at 952, and the center of the
 * missile is half of its 32 units further, at 936.
 */
constexpr float BYDO_MISSILE_START_X = 936.0F;

}  // namespace rtype::game::testing
