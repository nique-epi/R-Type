#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "HeldWeapon.hpp"

namespace rtype::game::testing {

/** @brief The numbers of the shipped ship and its missile, in the order of
 * the names of the forty-six shipped sheets: ship is the thirty-eighth,
 * player_missile the thirty-fourth. */
constexpr std::uint8_t SHIPPED_SHIP_NUMBER = 38;
constexpr std::uint8_t SHIPPED_PLAYER_MISSILE_NUMBER = 34;

/** @brief The gun of the shipped ship: 333 ms between shots is 20 ticks, the
 * first shot at once, its missile flying right at 960 units per second. */
constexpr std::uint16_t SHIPPED_GUN_INTERVAL_TICKS = 20;
constexpr std::uint16_t SHIPPED_GUN_FIRST_SHOT_TICKS = 0;
constexpr float SHIPPED_PLAYER_MISSILE_SPEED = 960.0F;

/** @brief The weapons of the shipped ship, red, blue and yellow, two levels
 * each, and where red and blue stand among them. */
constexpr std::size_t SHIPPED_WEAPON_COUNT = 3;
constexpr std::size_t SHIPPED_LEVELS_PER_WEAPON = 2;
constexpr std::size_t RED_PLACE = 0;
constexpr std::size_t BLUE_PLACE = 1;
constexpr std::size_t FIRST_LEVEL_PLACE = 0;
constexpr std::size_t SECOND_LEVEL_PLACE = 1;

/** @brief The second level of red: five red shots over 48 degrees, laid 16
 * units ahead, scattering at 960 units per second. */
constexpr std::uint8_t RED_SECOND_LEVEL_COUNT = 5;
constexpr float RED_SECOND_LEVEL_SPREAD_DEGREES = 48.0F;
constexpr float RED_SHOT_SPEED = 960.0F;

/** @brief The first level of blue: two blue shots 24 rows apart every 250
 * ms, 15 ticks, flying right at 1440 units per second. */
constexpr std::uint8_t BLUE_FIRST_LEVEL_COUNT = 2;
constexpr std::uint16_t BLUE_FIRST_LEVEL_INTERVAL_TICKS = 15;
constexpr float BLUE_SHOT_SPEED = 1440.0F;

/** @brief Held weapons: red at its second level, a weapon the ship does not
 * hold, and a level red does not have. */
constexpr HeldWeapon RED_AT_ITS_SECOND_LEVEL{.weapon = 1, .level = 2};
constexpr HeldWeapon A_WEAPON_PAST_THE_LAST{.weapon = 9, .level = 1};
constexpr HeldWeapon A_LEVEL_PAST_THE_LAST{.weapon = 1, .level = 3};

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
