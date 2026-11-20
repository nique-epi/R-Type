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
constexpr std::string_view SHEET_WITH_TWENTY_FIVE_FRAMES_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 25
spacing = 2
frame_ms = 120
)";
constexpr std::string_view SHEET_WITH_TWENTY_FOUR_FRAMES_TEXT = R"(
[gameplay]

[appearance]
sheet = "sprites/voltjelly.png"

[appearance.idle]
left = 10
top = 20
width = 8
height = 6
frames = 24
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
constexpr std::size_t MOST_FRAMES_A_SHEET_HOLDS = 24;

/** @brief A square of 3 by 3 drawn pixels, and what peeling its outline once
 * leaves: its center, the one pixel whose four neighbours are drawn. */
constexpr std::array<std::string_view, 3> FULL_SQUARE_OF_THREE{"###", "###",
                                                               "###"};
constexpr std::array<std::string_view, 3> CENTER_OF_A_SQUARE_OF_THREE{
    "...", ".#.", "..."};
constexpr int PEELED_ONCE = 1;

/** @brief The folder of the sheets the resource fosse brings, from the assets
 * folder of the sources, and the name of every sheet in it. */
constexpr std::string_view FOSSE_SHEETS_FROM_THE_ASSETS =
    "../resources/fosse/entities";
constexpr std::array<std::string_view, 15> FOSSE_SHEET_NAMES{
    "bone_arm",          "bone_spike",       "glowmoth",    "gloomwyrm",
    "gloomwyrm_segment", "gloomwyrm_tail",   "lanternfin",  "necromancer",
    "necromancer_beam",  "necromancer_ring", "ruin_warden", "ruin_warden_bolt",
    "sporecap",          "sporecap_spore",   "stalagmaw"};

/** @brief The folder of the sheets the resource ashlands brings, from the
 * assets folder of the sources, and the name of every sheet in it. */
constexpr std::string_view ASHLANDS_SHEETS_FROM_THE_ASSETS =
    "../resources/ashlands/entities";
constexpr std::array<std::string_view, 41> ASHLANDS_SHEET_NAMES{
    "alarm_beacon",
    "ash_egg",
    "ash_mother",
    "atomic_breath",
    "billboard",
    "billboard_wreck",
    "blast_door",
    "blast_door_lower",
    "brood_mite",
    "cindermite",
    "control_rods",
    "coolant_leak",
    "dunecrawler",
    "dunecrawler_barb",
    "fixbot",
    "fixbot_wrench",
    "geiger_mine",
    "geiger_mine_shard",
    "glowheart",
    "glowheart_drop",
    "glowheart_mite",
    "glowheart_tendril_high",
    "glowheart_tendril_low",
    "glowheart_tendril_middle",
    "live_cable",
    "meltdown_beam",
    "meltdown_lamp",
    "rad_geyser",
    "rail_turret",
    "rail_turret_shot",
    "sand_debris",
    "sandmaw",
    "sandmaw_boulder",
    "scrapdrone",
    "scrapdrone_bolt",
    "searchlight",
    "steam_vent",
    "toxic_barrel",
    "toxic_blast",
    "toxic_puddle",
    "vent_grille"};

/** @brief The sheet of the boss of the Pit, and the name its bar writes. */
constexpr std::string_view NECROMANCER_SHEET = "necromancer";
constexpr std::string_view NECROMANCER_BOSS_NAME = "NECROMANCER";
