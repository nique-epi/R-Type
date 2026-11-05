#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/** @brief A sheet with two attacks: the strike of the first holds its last
 * frame, goes back to the idle animation and hangs 12 pixels to the left of the
 * body; the signal of the second plays once, stays on its last frame and has
 * no anchor. */
constexpr std::string_view SHEET_WITH_ATTACKS_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_1.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "hold"
afterwards = "idle"
anchor_x = -12
anchor_y = 0

[appearance.attack_2.signal]
left = 0
top = 130
width = 16
height = 16
frames = 2
spacing = 0
frame_ms = 200
playback = "once"
afterwards = "stay"

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";

/** @brief The frames of the strike of the first attack: four frames of 48 by
 * 24 pixels from (0, 96), 2 pixels apart, so at left 0, 50, 100 and 150, each
 * 80 ms. */
constexpr std::array STRIKE_FRAMES{
    rtype::game::SpriteFrame{.left = 0, .top = 96, .width = 48, .height = 24},
    rtype::game::SpriteFrame{.left = 50, .top = 96, .width = 48, .height = 24},
    rtype::game::SpriteFrame{.left = 100, .top = 96, .width = 48, .height = 24},
    rtype::game::SpriteFrame{
        .left = 150, .top = 96, .width = 48, .height = 24}};
constexpr rtype::engine::Duration STRIKE_FRAME_TIME =
    std::chrono::milliseconds(80);

/** @brief The attack numbers of the sheet above. */
constexpr std::uint8_t FIRST_ATTACK = 1;
constexpr std::uint8_t SECOND_ATTACK = 2;

/** @brief The strike of the first attack, but its `afterwards` left out. */
constexpr std::string_view SHEET_WITHOUT_AFTERWARDS_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_1.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "once"

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";

/** @brief Sheets each broken in one way, the rest as the sheet above. */
constexpr std::string_view SHEET_WITH_A_NAMELESS_ATTACK_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_first.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "once"

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";
constexpr std::string_view SHEET_WITH_ATTACK_256_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_256.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "once"

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";
constexpr std::string_view SHEET_WITH_AN_UNKNOWN_PHASE_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_1.windup]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "once"

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";
constexpr std::string_view SHEET_WITHOUT_PLAYBACK_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_1.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";
constexpr std::string_view SHEET_WITH_AN_UNKNOWN_PLAYBACK_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_1.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "bounce"

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";
constexpr std::string_view SHEET_WITH_AN_UNKNOWN_AFTERWARDS_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_1.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "once"
afterwards = "forever"

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";
constexpr std::string_view SHEET_WITH_A_FAR_ANCHOR_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_1.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "once"
anchor_x = -257

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";
constexpr std::string_view SHEET_WITH_AN_UNKNOWN_ACTION_KEY_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.attack_1.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "once"
speed = 3

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";
