#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The numbers of the shipped ship and its missile, in the order of
 * the names of the eleven shipped sheets: ship is the ninth, player_missile
 * the sixth. */
constexpr std::uint8_t SHIPPED_SHIP_NUMBER = 9;
constexpr std::uint8_t SHIPPED_PLAYER_MISSILE_NUMBER = 6;

/** @brief The gun of the shipped ship: 333 ms between shots is 20 ticks, the
 * first shot at once, its missile flying right at 960 units per second. */
constexpr std::uint16_t SHIPPED_GUN_INTERVAL_TICKS = 20;
constexpr std::uint16_t SHIPPED_GUN_FIRST_SHOT_TICKS = 0;
constexpr float SHIPPED_PLAYER_MISSILE_SPEED = 960.0F;

/** @brief A ship that fires nothing. */
constexpr std::string_view UNARMED_SHIP_SHEET_TEXT = R"([gameplay]
side = "player"
pilot = "player"
[gameplay.hitbox]
width = 128
height = 64
[gameplay.motion]
kind = "ship"
[appearance]
)";

}  // namespace rtype::server::testing
