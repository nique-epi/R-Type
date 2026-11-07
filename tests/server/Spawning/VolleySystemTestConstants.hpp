#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief The sheets of the volley tests: an enemy that lays a column of
 * three minions in the strike of its attack 1, 100 units left of its center
 * and 50 units apart going down, and the minion. */
constexpr std::string_view BROODER_NAME_IN_TESTS = "brooder";
constexpr std::string_view MINION_NAME_IN_TESTS = "minion";
constexpr std::string_view BROODER_SHEET_TEXT = R"([gameplay]
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
[[gameplay.volley]]
attack = 1
phase = "strike"
body = "minion"
count = 3
offset_x = -100
offset_y = 0
step_y = 50
[appearance]
)";
constexpr std::string_view MINION_SHEET_TEXT = R"([gameplay]
side = "enemy"
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

/** @brief The action the brooder fires its volley on, and another one. */
constexpr game::ActionKey BROOD_STRIKE{.attack = 1,
                                       .phase = game::EnemyPhase::Strike};
constexpr game::ActionKey BROOD_SIGNAL{.attack = 1,
                                       .phase = game::EnemyPhase::Signal};

/** @brief Where the brooder stands, and where its three minions appear:
 * 100 units to its left, then 50 and 100 units lower. */
constexpr game::Position BROODER_CENTER{.x = 1000.0F, .y = 500.0F};
constexpr game::Position FIRST_MINION{.x = 900.0F, .y = 500.0F};
constexpr game::Position SECOND_MINION{.x = 900.0F, .y = 550.0F};
constexpr game::Position THIRD_MINION{.x = 900.0F, .y = 600.0F};
constexpr std::size_t MINIONS_PER_VOLLEY = 3;
constexpr std::array<game::Position, MINIONS_PER_VOLLEY> MINION_COLUMN{
    FIRST_MINION, SECOND_MINION, THIRD_MINION};

/** @brief The players of the volley tests, by the order they connect in. */
constexpr std::size_t FIRST_VOLLEY_PLAYER = 0;
constexpr std::size_t SECOND_VOLLEY_PLAYER = 1;
constexpr std::size_t VOLLEY_PLAYER_COUNT = 2;

}  // namespace rtype::server::testing
