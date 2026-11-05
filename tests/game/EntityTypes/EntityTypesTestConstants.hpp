#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief Today's values of the entities of the game, which the shipped
 * sheets must keep: a Bydo is 96 by 96, crosses at 240 logical units per
 * second, has 3 health points and fires every 1500 ms, first 1500 ms after it
 * appeared (90 ticks); a Drifter is worth 100 points and a Weaver 200, weaving
 * 48 units every 2000 ms; a ship is 128 by 64 and fires 3 times per second (20
 * ticks), at once; a missile of a player is 64 by 24 and flies right at 960, a
 * missile of an enemy 32 by 32, left at 360. */
constexpr float REQUIRED_BYDO_SIZE = 96.0F;
constexpr float REQUIRED_BYDO_SPEED = 240.0F;
constexpr int REQUIRED_BYDO_HEALTH = 3;
constexpr std::size_t REQUIRED_BYDO_SHOT_TICKS = 90;
constexpr std::uint32_t REQUIRED_DRIFTER_POINTS = 100;
constexpr std::uint32_t REQUIRED_WEAVER_POINTS = 200;
constexpr std::array<std::int64_t, 1> REQUIRED_LEFT_HEADING{-1};
constexpr std::array<std::int64_t, 1> REQUIRED_RIGHT_HEADING{1};
constexpr std::array<std::int64_t, 2> REQUIRED_WEAVE{48, 2000};
constexpr float REQUIRED_SHIP_WIDTH = 128.0F;
constexpr float REQUIRED_SHIP_HEIGHT = 64.0F;
constexpr std::size_t REQUIRED_SHIP_SHOT_TICKS = 20;
constexpr std::size_t REQUIRED_SHIP_FIRST_SHOT_TICKS = 0;
constexpr float REQUIRED_PLAYER_MISSILE_WIDTH = 64.0F;
constexpr float REQUIRED_PLAYER_MISSILE_HEIGHT = 24.0F;
constexpr float REQUIRED_PLAYER_MISSILE_SPEED = 960.0F;
constexpr float REQUIRED_ENEMY_MISSILE_SIZE = 32.0F;
constexpr float REQUIRED_ENEMY_MISSILE_SPEED = 360.0F;
constexpr std::string_view ENEMY_MISSILE_NAME_IN_TESTS = "enemy_missile";
constexpr std::string_view REQUIRED_STRAIGHT_NAME = "straight";
constexpr std::string_view REQUIRED_WEAVE_NAME = "weave";

}  // namespace rtype::game::testing
