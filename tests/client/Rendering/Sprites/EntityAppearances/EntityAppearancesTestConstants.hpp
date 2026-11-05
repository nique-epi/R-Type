#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/** @brief The name of the sheet the reading tests read, and a name no sheet
 * has. */
constexpr std::string_view READ_TYPE_NAME = "voltjelly";
constexpr std::string_view UNREAD_TYPE_NAME = "reaper";

/** @brief The sprite sheet of the sheet the reading tests read, and another
 * one. */
constexpr std::string_view READ_SHEET_ID = "sprites/voltjelly.png";
constexpr std::string_view OTHER_SHEET_ID = "sprites/reaper.png";

/**
 * @brief The idle animation of the sheet the reading tests read: three frames
 * of 8 by 6 pixels from (10, 20), 2 pixels apart, so at left 10, 20 and 30,
 * each 120 ms.
 */
constexpr std::array READ_IDLE_FRAMES{
    rtype::game::SpriteFrame{.left = 10, .top = 20, .width = 8, .height = 6},
    rtype::game::SpriteFrame{.left = 20, .top = 20, .width = 8, .height = 6},
    rtype::game::SpriteFrame{.left = 30, .top = 20, .width = 8, .height = 6}};
constexpr rtype::engine::Duration READ_FRAME_TIME =
    std::chrono::milliseconds(120);

/** @brief A whole sheet, as the reading tests read it: a `[gameplay]` the
 * client does not read, and the appearance above. */
constexpr std::string_view READ_SHEET_TEXT = R"(
[gameplay]
whatever = "the client never reads it"

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 3
spacing = 2
frame_ms = 120
)";

/** @brief A sheet that draws from OTHER_SHEET_ID. */
constexpr std::string_view OTHER_SHEET_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/reaper.png"

[appearance.idle]
left = 0
top = 0
width = 8
height = 8
frames = 1
spacing = 0
frame_ms = 100
)";

/** @brief Sheets each broken in one way, the rest as READ_SHEET_TEXT. */
constexpr std::string_view SHEET_WITHOUT_IDLE_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"
)";
constexpr std::string_view SHEET_WITHOUT_GAMEPLAY_TEXT = R"(
[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 3
spacing = 2
frame_ms = 120
)";
constexpr std::string_view SHEET_WITH_AN_UNKNOWN_ROOT_TABLE_TEXT = R"(
[gameplay]

[sound]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 3
spacing = 2
frame_ms = 120
)";
constexpr std::string_view SHEET_WITH_AN_UNKNOWN_APPEARANCE_KEY_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"
tint = 3

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 3
spacing = 2
frame_ms = 120
)";
constexpr std::string_view SHEET_WITH_AN_UNKNOWN_IDLE_KEY_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 3
spacing = 2
frame_ms = 120
loop = 1
)";
constexpr std::string_view SHEET_WITH_NO_FRAME_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 0
spacing = 2
frame_ms = 120
)";
constexpr std::string_view SHEET_WITH_SEVENTEEN_FRAMES_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 17
spacing = 2
frame_ms = 120
)";
constexpr std::string_view SHEET_WITH_SIXTEEN_FRAMES_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 16
spacing = 2
frame_ms = 120
)";
constexpr std::string_view SHEET_WITH_A_FRACTIONAL_FRAME_TIME_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 3
spacing = 2
frame_ms = 120.0
)";
constexpr std::string_view SHEET_WITH_A_NUMBER_AS_ITS_SHEET_TEXT = R"(
[gameplay]

[appearance]
sheet = 7

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 3
spacing = 2
frame_ms = 120
)";

/** @brief The most frames an idle animation holds, as the sheet format of
 * the architecture page writes it. */
constexpr std::size_t MOST_FRAMES_A_SHEET_HOLDS = 16;

/** @brief The Voltjelly as its design draws it: a frame of 40 by 40 sheet
 * pixels holding its aura, around a bell of 24 by 24. */
constexpr int VOLTJELLY_FRAME_SIDE = 40;
constexpr int VOLTJELLY_BELL_SIDE = 24;
