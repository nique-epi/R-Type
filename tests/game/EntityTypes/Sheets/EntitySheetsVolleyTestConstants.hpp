#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game::testing {

/** @brief The sheet of an enemy whose volley, in the strike of its attack 1,
 * is made of three bodies of the sheet `minion`; of the minion, an enemy;
 * and of a brooder whose volley is made of missiles of a player. */
constexpr std::string_view BROODER_NAME_IN_SHEET_TESTS = "brooder";
constexpr std::string_view MINION_NAME_IN_SHEET_TESTS = "minion";
constexpr std::string_view BROODER_OF_MINIONS_TEXT = R"([gameplay]
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
[[gameplay.volley]]
attack = 1
phase = "strike"
body = "minion"
count = 3
offset_x = -100
offset_y = 0
[appearance]
)";
constexpr std::string_view MINION_SHEET_TEXT = R"([gameplay]
side = "enemy"
layer = "enemy"
pilot = "straight"
[gameplay.hitbox]
width = 64
height = 64
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[appearance]
)";
constexpr std::string_view BROODER_OF_PLAYER_MISSILES_TEXT = R"([gameplay]
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
[[gameplay.volley]]
attack = 1
phase = "strike"
body = "player_missile"
count = 3
offset_x = -100
offset_y = 0
[appearance]
)";

/** @brief The volleys of the shipped Bydo Tyrant: its calm and enraged
 * bursts (2), the six mobs of its calm brood and the six of its enraged one
 * (12), its spores and its wall (2). */
constexpr std::size_t VOLLEYS_OF_THE_TYRANT = 16;

}  // namespace rtype::game::testing
