#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief Sheets the catalog tests read, with today's values. */
constexpr std::string_view DRIFTER_SHEET_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
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
layer = "enemy"
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
layer = "enemy_missile"
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
layer = "player_missile"
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
/** @brief An orb, a missile of an enemy that bursts after 100 ms into four
 * shards of the sheet `shard`, and that shard, which scatters. */
constexpr std::string_view ORB_NAME_IN_TESTS = "orb";
constexpr std::string_view SHARD_NAME_IN_TESTS = "shard";
constexpr std::string_view ORB_SHEET_TEXT = R"([gameplay]
side = "enemy_missile"
layer = "enemy_missile"
pilot = "straight"
[gameplay.hitbox]
width = 36
height = 36
[gameplay.motion]
kind = "steady"
top_speed = 360
[gameplay.behaviour]
heading = -1
[gameplay.burst]
after_ms = 100
count = 4
shard = "shard"
[appearance]
)";
constexpr std::string_view SHARD_SHEET_TEXT = R"([gameplay]
side = "enemy_missile"
layer = "enemy_missile"
pilot = "scatter"
[gameplay.hitbox]
width = 20
height = 20
[gameplay.motion]
kind = "steady"
top_speed = 300
[appearance]
)";
constexpr std::string_view ORB_BURSTING_INTO_PLAYER_MISSILES_TEXT =
    R"([gameplay]
side = "enemy_missile"
layer = "enemy_missile"
pilot = "straight"
[gameplay.hitbox]
width = 36
height = 36
[gameplay.motion]
kind = "steady"
top_speed = 360
[gameplay.behaviour]
heading = -1
[gameplay.burst]
after_ms = 100
count = 4
shard = "player_missile"
[appearance]
)";
constexpr std::string_view DRIFTER_THAT_BURSTS_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[gameplay.burst]
after_ms = 100
count = 4
shard = "shard"
[appearance]
)";

/** @brief When the orb bursts, in ticks: 100 ms is 6 ticks of 16.7 ms, the
 * nearest whole count; and how many shards it bursts into. */
constexpr std::size_t ORB_BURST_TICKS = 6;
constexpr std::uint8_t SHARDS_PER_ORB = 4;

constexpr std::string_view DRIFTER_FIRING_PLAYER_MISSILES_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
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

/** @brief The most sheets a program loads, as the issue raising the cap
 * asks: a type number is one byte and 0 names none. Then one more, and the
 * number the last of the most sheets takes. */
constexpr std::size_t MOST_SHEETS_LOADED = 255;
constexpr std::size_t ONE_SHEET_TOO_MANY = 256;
constexpr std::uint8_t LAST_TYPE_NUMBER = 255;

/** @brief The numbers of three sheets given as weaver, drifter and
 * enemy_missile: their place in the order of the names, from 1. */
constexpr std::uint8_t DRIFTER_NUMBER_AMONG_THREE = 1;
constexpr std::uint8_t ENEMY_MISSILE_NUMBER_AMONG_THREE = 2;
constexpr std::uint8_t WEAVER_NUMBER_AMONG_THREE = 3;
constexpr std::uint8_t NO_SHEET_NUMBER = 0;
constexpr std::uint8_t NUMBER_PAST_THE_THREE = 4;

/** @brief The first part of the names of generated sheets. */
constexpr std::string_view GENERATED_SHEET_PREFIX = "missile_";

}  // namespace rtype::game::testing
