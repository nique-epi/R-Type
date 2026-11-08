#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/**
 * @brief A sheet that draws its death: its idle animation is two frames of 24
 * by 24 pixels from the top left corner, its death three frames of 48 by 48
 * pixels from (0, 26), 2 pixels apart, so at left 0, 50 and 100, each 70 ms.
 * The death without flash lies in the row right under it, 48 + 2 pixels
 * lower, at top 76.
 */
constexpr std::string_view SHEET_WITH_A_DEATH_TEXT = R"(
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

[appearance.death]
left = 0
top = 26
width = 48
height = 48
frames = 3
spacing = 2
frame_ms = 70
)";
constexpr std::array DEATH_FRAMES{
    rtype::game::SpriteFrame{.left = 0, .top = 26, .width = 48, .height = 48},
    rtype::game::SpriteFrame{.left = 50, .top = 26, .width = 48, .height = 48},
    rtype::game::SpriteFrame{
        .left = 100, .top = 26, .width = 48, .height = 48}};
constexpr std::array DEATH_FRAMES_WITHOUT_FLASH{
    rtype::game::SpriteFrame{.left = 0, .top = 76, .width = 48, .height = 48},
    rtype::game::SpriteFrame{.left = 50, .top = 76, .width = 48, .height = 48},
    rtype::game::SpriteFrame{
        .left = 100, .top = 76, .width = 48, .height = 48}};
constexpr rtype::engine::Duration DEATH_FRAME_TIME =
    std::chrono::milliseconds(70);

/** @brief The same sheet whose death holds a key no idle has. */
constexpr std::string_view DEATH_WITH_AN_UNKNOWN_KEY_TEXT = R"(
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

[appearance.death]
left = 0
top = 26
width = 48
height = 48
frames = 3
spacing = 2
frame_ms = 70
playback = "once"
)";

/** @brief The shipped enemies whose sheet draws their own death, the frames
 * each death holds, as the Bydo explosion, and the sheet of the Bydo, which
 * draws none. */
constexpr std::array<std::string_view, 4> ENEMIES_WITH_A_DEATH{
    "voltjelly", "reaper", "drillback", "bydomancer"};
constexpr std::size_t SHIPPED_DEATH_FRAME_COUNT = 6;
constexpr std::string_view BYDO_SHEET_NAME = "drifter";
