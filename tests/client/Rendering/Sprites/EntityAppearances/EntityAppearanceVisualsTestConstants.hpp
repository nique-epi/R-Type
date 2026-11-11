#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/**
 * @brief A sheet naming one effect and two looks. The effect `spark` is two
 * frames of 16 by 16 pixels from (0, 30), 4 pixels apart, so at left 0 and
 * 20, each 60 ms, hanging 8 pixels above the body; its version without flash
 * lies in the row right under it, 16 + 4 pixels lower, at top 50. The looks
 * are given out of the order of their names: `cracked`, three frames of 24 by
 * 24 pixels from (0, 70), 2 pixels apart, each 120 ms, then `angry`, one
 * frame at (0, 100), 300 ms.
 */
constexpr std::string_view SHEET_WITH_VISUALS_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/mobs/reaper.png"

[appearance.idle]
left = 0
top = 0
width = 24
height = 24
frames = 2
spacing = 2
frame_ms = 250

[appearance.effects.spark]
left = 0
top = 30
width = 16
height = 16
frames = 2
spacing = 4
frame_ms = 60
anchor_y = -8

[appearance.looks.cracked]
left = 0
top = 70
width = 24
height = 24
frames = 3
spacing = 2
frame_ms = 120

[appearance.looks.angry]
left = 0
top = 100
width = 24
height = 24
frames = 1
spacing = 0
frame_ms = 300
)";
constexpr std::string_view SPARK_EFFECT = "spark";
constexpr std::array SPARK_FRAMES{
    rtype::game::SpriteFrame{.left = 0, .top = 30, .width = 16, .height = 16},
    rtype::game::SpriteFrame{.left = 20, .top = 30, .width = 16, .height = 16}};
constexpr std::array SPARK_FRAMES_WITHOUT_FLASH{
    rtype::game::SpriteFrame{.left = 0, .top = 50, .width = 16, .height = 16},
    rtype::game::SpriteFrame{.left = 20, .top = 50, .width = 16, .height = 16}};
constexpr rtype::engine::Duration SPARK_FRAME_TIME =
    std::chrono::milliseconds(60);
constexpr int SPARK_ANCHOR_Y = -8;

/** @brief The looks in the order of their names: `angry` first, numbered 1,
 * then `cracked`, numbered 2. */
constexpr std::size_t TWO_LOOKS = 2;
constexpr std::array ANGRY_FRAMES{
    rtype::game::SpriteFrame{.left = 0, .top = 100, .width = 24, .height = 24}};
constexpr std::array CRACKED_FRAMES{
    rtype::game::SpriteFrame{.left = 0, .top = 70, .width = 24, .height = 24},
    rtype::game::SpriteFrame{.left = 26, .top = 70, .width = 24, .height = 24},
    rtype::game::SpriteFrame{.left = 52, .top = 70, .width = 24, .height = 24}};
constexpr rtype::engine::Duration CRACKED_FRAME_TIME =
    std::chrono::milliseconds(120);

/** @brief The same sheet whose effect holds a key no effect has. */
constexpr std::string_view EFFECT_WITH_AN_UNKNOWN_KEY_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/mobs/reaper.png"

[appearance.idle]
left = 0
top = 0
width = 24
height = 24
frames = 2
spacing = 2
frame_ms = 250

[appearance.effects.spark]
left = 0
top = 30
width = 16
height = 16
frames = 2
spacing = 4
frame_ms = 60
playback = "once"
)";
