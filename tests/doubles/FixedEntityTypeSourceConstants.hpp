#pragma once

#include <array>
#include <cstdint>
#include "CollisionBox.hpp"

namespace rtype::client::testing {

/**
 * @brief The numbers the server gives the seven sheets the double knows, in
 * the order of their names, from 1: drifter, enemy_missile, player_missile,
 * ship, weaver, a look item and an enemy of two boxes; then the first number
 * past them, which names no type.
 */
constexpr std::uint8_t DRIFTER_TYPE_NUMBER = 1;
constexpr std::uint8_t ENEMY_MISSILE_TYPE_NUMBER = 2;
constexpr std::uint8_t PLAYER_MISSILE_TYPE_NUMBER = 3;
constexpr std::uint8_t SHIP_TYPE_NUMBER = 4;
constexpr std::uint8_t WEAVER_TYPE_NUMBER = 5;
constexpr std::uint8_t LOOK_ITEM_TYPE_NUMBER = 6;
constexpr std::uint8_t TWO_BOX_ENEMY_TYPE_NUMBER = 7;
constexpr std::uint8_t NUMBER_OF_NO_TYPE = 8;

/**
 * @brief The boxes of the shipped sheets, by side: a Bydo of 96 by 96, a
 * missile of an enemy of 32 by 32, a missile of a player of 64 by 24 and a
 * ship of 128 by 64.
 */
constexpr game::CollisionBox BYDO_BOX{.width = 96.0F, .height = 96.0F};
constexpr game::CollisionBox ENEMY_MISSILE_BOX{.width = 32.0F, .height = 32.0F};
constexpr game::CollisionBox PLAYER_MISSILE_BOX{.width = 64.0F,
                                                .height = 24.0F};
constexpr game::CollisionBox SHIP_BOX{.width = 128.0F, .height = 64.0F};

/** @brief The two boxes of the enemy of two boxes, each the box of a Bydo,
 * 200 units above and 200 below its Position: nothing of it lies from 152
 * above to 152 below. */
constexpr float TWO_BOX_ENEMY_PART_DISTANCE = 200.0F;
constexpr std::array TWO_BOX_ENEMY_PARTS{
    game::CollisionBox{.width = 96.0F,
                       .height = 96.0F,
                       .offsetY = -TWO_BOX_ENEMY_PART_DISTANCE},
    game::CollisionBox{.width = 96.0F,
                       .height = 96.0F,
                       .offsetY = TWO_BOX_ENEMY_PART_DISTANCE}};

/** @brief The gun of the shipped ship: one missile every 20 ticks from the
 * start, flying right at 960 units per second. */
constexpr std::uint16_t SHIP_GUN_INTERVAL_TICKS = 20;
constexpr float PLAYER_MISSILE_SPEED = 960.0F;

}  // namespace rtype::client::testing
