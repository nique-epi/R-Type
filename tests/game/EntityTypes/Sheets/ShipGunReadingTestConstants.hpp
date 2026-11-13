#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief The numbers of the shipped ship and its missile, in the order of
 * the names of the thirty-eight shipped sheets: ship is the thirty-second,
 * player_missile the twenty-ninth. */
constexpr std::uint8_t SHIPPED_SHIP_NUMBER = 32;
constexpr std::uint8_t SHIPPED_PLAYER_MISSILE_NUMBER = 29;

/** @brief The gun of the shipped ship: 333 ms between shots is 20 ticks, the
 * first shot at once, its missile flying right at 960 units per second. */
constexpr std::uint16_t SHIPPED_GUN_INTERVAL_TICKS = 20;
constexpr std::uint16_t SHIPPED_GUN_FIRST_SHOT_TICKS = 0;
constexpr float SHIPPED_PLAYER_MISSILE_SPEED = 960.0F;

/** @brief A ship that fires nothing. */
constexpr std::string_view UNARMED_SHIP_SHEET_TEXT = R"([gameplay]
side = "player"
layer = "player"
pilot = "player"
[gameplay.hitbox]
width = 128
height = 64
[gameplay.motion]
kind = "ship"
[appearance]
)";

/** @brief A Bydo whose pilot is a behaviour only a plugin of its server
 * knows, so its sheet does not read as EntitySheets on a client. */
constexpr std::string_view PLUGIN_PILOT_SHEET_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
pilot = "lancer_charge"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 120
[gameplay.behaviour]
windup_ms = 400
[appearance]
)";

}  // namespace rtype::game::testing
