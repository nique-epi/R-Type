#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief Sheets the catalog tests read, with today's values. */
constexpr std::string_view DRIFTER_SHEET_TEXT = R"([gameplay]
side = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[gameplay.gun]
interval_ms = 1500
first_shot_ms = 1500
projectile = "enemy_missile"
[appearance]
)";
constexpr std::string_view WEAVER_SHEET_TEXT = R"([gameplay]
side = "enemy"
pilot = "weave"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
amplitude = 48
period_ms = 2000
[gameplay.gun]
interval_ms = 1500
first_shot_ms = 1500
projectile = "enemy_missile"
[appearance]
)";
constexpr std::string_view ENEMY_MISSILE_SHEET_TEXT = R"([gameplay]
side = "enemy_missile"
pilot = "straight"
[gameplay.hitbox]
width = 32
height = 32
[gameplay.motion]
kind = "steady"
top_speed = 360
[gameplay.behaviour]
heading = -1
[appearance]
)";
constexpr std::string_view PLAYER_MISSILE_SHEET_TEXT = R"([gameplay]
side = "player_missile"
pilot = "straight"
[gameplay.hitbox]
width = 64
height = 24
[gameplay.motion]
kind = "steady"
top_speed = 960
[gameplay.behaviour]
heading = 1
[appearance]
)";
constexpr std::string_view DRIFTER_FIRING_PLAYER_MISSILES_TEXT = R"([gameplay]
side = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[gameplay.gun]
interval_ms = 1500
first_shot_ms = 1500
projectile = "player_missile"
[appearance]
)";

/** @brief The names of those sheets, and names a type may or may not have. */
constexpr std::string_view DRIFTER_NAME_IN_TESTS = "drifter";
constexpr std::string_view WEAVER_NAME_IN_TESTS = "weaver";
constexpr std::string_view PLAYER_MISSILE_NAME_IN_TESTS = "player_missile";
constexpr std::string_view SHIP_NAME_IN_TESTS = "ship";
constexpr std::string_view NAME_OF_NO_SHEET = "voltjelly";
constexpr std::string_view NAME_WITH_AN_UPPERCASE_LETTER = "Drifter";
constexpr std::string_view EMPTY_NAME{};
constexpr std::string_view NAME_OF_24_BYTES = "abcdefghijklmnopqrstuvwx";
constexpr std::string_view NAME_OF_25_BYTES = "abcdefghijklmnopqrstuvwxy";

/** @brief The most sheets a program loads, as the protocol page bounds the
 * table of the types, and one more. */
constexpr std::size_t MOST_SHEETS_LOADED = 32;
constexpr std::size_t ONE_SHEET_TOO_MANY = 33;

/** @brief The numbers of three sheets given as weaver, drifter and
 * enemy_missile: their place in the order of the names, from 1. */
constexpr std::uint8_t DRIFTER_NUMBER_AMONG_THREE = 1;
constexpr std::uint8_t ENEMY_MISSILE_NUMBER_AMONG_THREE = 2;
constexpr std::uint8_t WEAVER_NUMBER_AMONG_THREE = 3;
constexpr std::uint8_t NO_SHEET_NUMBER = 0;
constexpr std::uint8_t NUMBER_PAST_THE_THREE = 4;

/** @brief The names of the sheets the game ships with, one per type of today,
 * in the order of their names. */
constexpr std::string_view SHIPPED_DRIFTER_NAME = "drifter";
constexpr std::string_view SHIPPED_ENEMY_MISSILE_NAME = "enemy_missile";
constexpr std::string_view SHIPPED_PLAYER_MISSILE_NAME = "player_missile";
constexpr std::string_view SHIPPED_SHIP_NAME = "ship";
constexpr std::string_view SHIPPED_WEAVER_NAME = "weaver";
constexpr std::array<std::string_view, 5> SHIPPED_SHEET_NAMES{
    SHIPPED_DRIFTER_NAME, SHIPPED_ENEMY_MISSILE_NAME,
    SHIPPED_PLAYER_MISSILE_NAME, SHIPPED_SHIP_NAME, SHIPPED_WEAVER_NAME};

/** @brief The first part of the names of generated sheets. */
constexpr std::string_view GENERATED_SHEET_PREFIX = "missile_";

}  // namespace rtype::game::testing
