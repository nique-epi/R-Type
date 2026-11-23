#pragma once

#include <array>
#include <cstdint>
#include "CollisionBox.hpp"
#include "HeldWeapon.hpp"
#include "ShotPattern.hpp"

namespace rtype::client::testing {

/**
 * @brief The numbers the server gives the nine sheets the double knows, in
 * the order of their names, from 1: drifter, enemy_missile, player_missile,
 * ship, weaver, a look item, an enemy of two boxes, a hollow enemy and a
 * blocking wall; then
 * the first number past them, which names no type.
 */
constexpr std::uint8_t DRIFTER_TYPE_NUMBER = 1;
constexpr std::uint8_t ENEMY_MISSILE_TYPE_NUMBER = 2;
constexpr std::uint8_t PLAYER_MISSILE_TYPE_NUMBER = 3;
constexpr std::uint8_t SHIP_TYPE_NUMBER = 4;
constexpr std::uint8_t WEAVER_TYPE_NUMBER = 5;
constexpr std::uint8_t LOOK_ITEM_TYPE_NUMBER = 6;
constexpr std::uint8_t TWO_BOX_ENEMY_TYPE_NUMBER = 7;
constexpr std::uint8_t HOLLOW_ENEMY_TYPE_NUMBER = 8;
constexpr std::uint8_t BLOCKING_WALL_TYPE_NUMBER = 9;
constexpr std::uint8_t NUMBER_OF_NO_TYPE = 10;

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

/** @brief The collision mask of the hollow enemy, as large as BYDO_BOX (96 /
 * 4 = 24 pixels a side), its HOLLOW_ENEMY_DRAWN_ROWS top rows alone drawn. */
constexpr int HOLLOW_ENEMY_SIDE = 24;
constexpr int HOLLOW_ENEMY_DRAWN_ROWS = 4;

/** @brief The gun of the shipped ship: one missile every 20 ticks from the
 * start, flying right at 960 units per second. */
constexpr std::uint16_t SHIP_GUN_INTERVAL_TICKS = 20;
constexpr float PLAYER_MISSILE_SPEED = 960.0F;

/** @brief The two weapons of the ship of the double, one level each: a fan of
 * three missiles over 24 degrees, laid 16 units ahead, which scatter at the
 * speed of the missile every 20 ticks; and two rows 24 units apart, which fly
 * straight every 10 ticks. */
constexpr game::HeldWeapon FAN_WEAPON{.weapon = 1, .level = 1};
constexpr game::HeldWeapon ROWS_WEAPON{.weapon = 2, .level = 1};
constexpr std::uint8_t FAN_MISSILES = 3;
constexpr float FAN_SPREAD_DEGREES = 24.0F;
constexpr float FAN_DISTANCE = 16.0F;
constexpr game::ShotPattern FAN_PATTERN{.count = FAN_MISSILES,
                                        .spreadDegrees = FAN_SPREAD_DEGREES,
                                        .stepY = 0.0F,
                                        .distance = FAN_DISTANCE};
constexpr std::uint8_t ROWS_MISSILES = 2;
constexpr float ROWS_APART = 24.0F;
constexpr game::ShotPattern ROWS_PATTERN{
    .count = ROWS_MISSILES, .spreadDegrees = 0.0F, .stepY = ROWS_APART};
constexpr std::uint16_t ROWS_GUN_INTERVAL_TICKS = 10;

}  // namespace rtype::client::testing
