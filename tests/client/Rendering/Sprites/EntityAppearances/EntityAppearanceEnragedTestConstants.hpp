#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/**
 * @brief A sheet with an enraged skin: its idle animation is two frames of
 * 128 by 128 pixels from the top left corner, its enraged idle two frames of
 * the same size from (0, 130), 2 pixels apart, so at left 0 and 130, each
 * 120 ms.
 */
constexpr std::string_view SHEET_WITH_AN_ENRAGED_IDLE_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/mobs/bydo_tyrant.png"

[appearance.idle]
left = 0
top = 0
width = 128
height = 128
frames = 2
spacing = 2
frame_ms = 150

[appearance.enraged_idle]
left = 0
top = 130
width = 128
height = 128
frames = 2
spacing = 2
frame_ms = 120
)";
constexpr std::array ENRAGED_IDLE_FRAMES{
    rtype::game::SpriteFrame{
        .left = 0, .top = 130, .width = 128, .height = 128},
    rtype::game::SpriteFrame{
        .left = 130, .top = 130, .width = 128, .height = 128}};
constexpr rtype::engine::Duration ENRAGED_IDLE_FRAME_TIME =
    std::chrono::milliseconds(120);

/** @brief The same sheet whose enraged idle holds a key no idle has. */
constexpr std::string_view ENRAGED_IDLE_WITH_AN_UNKNOWN_KEY_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/mobs/bydo_tyrant.png"

[appearance.idle]
left = 0
top = 0
width = 128
height = 128
frames = 2
spacing = 2
frame_ms = 150

[appearance.enraged_idle]
left = 0
top = 130
width = 128
height = 128
frames = 2
spacing = 2
frame_ms = 120
afterwards = "idle"
)";
