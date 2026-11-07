#pragma once

#include <string_view>

namespace rtype::game::testing {

/** @brief The name and the file of the sheet the reader tests read. */
constexpr std::string_view READ_SHEET_NAME = "test_sheet";
constexpr std::string_view READ_SHEET_SOURCE = "/sheets/test_sheet.toml";

/** @brief Pieces of a sheet, put one after the other: a test leaves one out,
 * or puts a wrong one in, to see the reader refuse it. The tables of
 * `[gameplay]` come after its two values, as TOML requires. */
constexpr std::string_view ENEMY_GAMEPLAY =
    "[gameplay]\nside = \"enemy\"\npilot = \"straight\"\n";
constexpr std::string_view WEAVER_GAMEPLAY =
    "[gameplay]\nside = \"enemy\"\npilot = \"weave\"\n";
constexpr std::string_view SHIP_GAMEPLAY =
    "[gameplay]\nside = \"player\"\npilot = \"player\"\n";
constexpr std::string_view MISSILE_GAMEPLAY =
    "[gameplay]\nside = \"player_missile\"\npilot = \"straight\"\n";
constexpr std::string_view ENEMY_MISSILE_GAMEPLAY =
    "[gameplay]\nside = \"enemy_missile\"\npilot = \"straight\"\n";
constexpr std::string_view LOOK_ITEM_GAMEPLAY =
    "[gameplay]\nside = \"look_item\"\nlook_item = \"crown\"\n"
    "pilot = \"straight\"\n";
constexpr std::string_view LOOK_ITEM_GAMEPLAY_WITHOUT_ITEM =
    "[gameplay]\nside = \"look_item\"\npilot = \"straight\"\n";
constexpr std::string_view LOOK_ITEM_OF_AN_UNKNOWN_NAME =
    "[gameplay]\nside = \"look_item\"\nlook_item = \"tiara\"\n"
    "pilot = \"straight\"\n";
constexpr std::string_view ENEMY_GAMEPLAY_WITH_AN_ITEM =
    "[gameplay]\nside = \"enemy\"\nlook_item = \"crown\"\n"
    "pilot = \"straight\"\n";

/** @brief The number of the crown among the items of the locker: its place,
 * 2, plus one. */
constexpr std::uint8_t CROWN_NUMBER = 3;
constexpr std::string_view WEAVING_MISSILE_GAMEPLAY =
    "[gameplay]\nside = \"player_missile\"\npilot = \"weave\"\n";
constexpr std::string_view GAMEPLAY_WITHOUT_SIDE =
    "[gameplay]\npilot = \"straight\"\n";
constexpr std::string_view GAMEPLAY_WITHOUT_PILOT =
    "[gameplay]\nside = \"enemy\"\n";
constexpr std::string_view GAMEPLAY_OF_AN_UNKNOWN_SIDE =
    "[gameplay]\nside = \"enmy\"\npilot = \"straight\"\n";
constexpr std::string_view GAMEPLAY_OF_AN_UNKNOWN_PILOT =
    "[gameplay]\nside = \"enemy\"\npilot = \"zigzag\"\n";
constexpr std::string_view ENEMY_PILOTED_BY_A_PLAYER =
    "[gameplay]\nside = \"enemy\"\npilot = \"player\"\n";
constexpr std::string_view PLAYER_PILOTED_BY_A_BEHAVIOUR =
    "[gameplay]\nside = \"player\"\npilot = \"straight\"\n";
constexpr std::string_view GAMEPLAY_WITH_AN_UNKNOWN_KEY =
    "[gameplay]\nside = \"enemy\"\npilot = \"straight\"\nlives = 3\n";
constexpr std::string_view BYDO_HITBOX =
    "[gameplay.hitbox]\nwidth = 96\nheight = 96\n";
constexpr std::string_view SHIP_HITBOX =
    "[gameplay.hitbox]\nwidth = 128\nheight = 64\n";
constexpr std::string_view NARROW_SHIP_HITBOX =
    "[gameplay.hitbox]\nwidth = 120\nheight = 64\n";
constexpr std::string_view EMPTY_HITBOX =
    "[gameplay.hitbox]\nwidth = 0\nheight = 96\n";
constexpr std::string_view HITBOX_WITHOUT_HEIGHT =
    "[gameplay.hitbox]\nwidth = 96\n";
constexpr std::string_view STEADY_MOTION =
    "[gameplay.motion]\nkind = \"steady\"\ntop_speed = 240\n";
constexpr std::string_view STEADY_MOTION_IN_A_FLOAT =
    "[gameplay.motion]\nkind = \"steady\"\ntop_speed = 240.0\n";
constexpr std::string_view STEADY_MOTION_WITHOUT_TOP_SPEED =
    "[gameplay.motion]\nkind = \"steady\"\n";
constexpr std::string_view SHIP_MOTION = "[gameplay.motion]\nkind = \"ship\"\n";
constexpr std::string_view SHIP_MOTION_WITH_A_TOP_SPEED =
    "[gameplay.motion]\nkind = \"ship\"\ntop_speed = 480\n";
constexpr std::string_view UNKNOWN_MOTION =
    "[gameplay.motion]\nkind = \"drift\"\ntop_speed = 240\n";
constexpr std::string_view LEFT_HEADING_TABLE =
    "[gameplay.behaviour]\nheading = -1\n";
constexpr std::string_view RIGHT_HEADING_TABLE =
    "[gameplay.behaviour]\nheading = 1\n";
constexpr std::string_view STILL_HEADING_TABLE =
    "[gameplay.behaviour]\nheading = 0\n";
constexpr std::string_view TOO_FAR_HEADING_TABLE =
    "[gameplay.behaviour]\nheading = 2\n";
constexpr std::string_view HEADING_TABLE_WITH_AN_UNKNOWN_SETTING =
    "[gameplay.behaviour]\nheading = -1\nspeed = 3\n";
constexpr std::string_view WEAVE_TABLE =
    "[gameplay.behaviour]\namplitude = 48\nperiod_ms = 2000\n";
constexpr std::string_view WEAVE_TABLE_PERIOD_FIRST =
    "[gameplay.behaviour]\nperiod_ms = 2000\namplitude = 48\n";
constexpr std::string_view WEAVE_TABLE_WITHOUT_PERIOD =
    "[gameplay.behaviour]\namplitude = 48\n";
constexpr std::string_view EMPTY_BEHAVIOUR_TABLE = "[gameplay.behaviour]\n";
constexpr std::string_view HEALTH_TABLE = "[gameplay.health]\npoints = 3\n";
constexpr std::string_view NO_HEALTH_TABLE = "[gameplay.health]\npoints = 0\n";
constexpr std::string_view SCORE_TABLE = "[gameplay.score]\npoints = 100\n";
constexpr std::string_view ENEMY_GUN =
    "[gameplay.gun]\ninterval_ms = 1500\nfirst_shot_ms = 1500\n"
    "projectile = \"enemy_missile\"\n";
constexpr std::string_view SHIP_GUN =
    "[gameplay.gun]\ninterval_ms = 333\nfirst_shot_ms = 0\n"
    "projectile = \"player_missile\"\n";
constexpr std::string_view GUN_WITHOUT_PROJECTILE =
    "[gameplay.gun]\ninterval_ms = 1500\nfirst_shot_ms = 1500\n";
constexpr std::string_view APPEARANCE = "[appearance]\n";
constexpr std::string_view APPEARANCE_AS_A_NUMBER = "appearance = 3\n";
constexpr std::string_view UNKNOWN_ROOT_TABLE = "[sound]\n";
constexpr std::string_view TEXT_THAT_IS_NOT_TOML = "[gameplay\n";

/** @brief The dotted key and the text a refused pilot rule is told by. */
constexpr std::string_view PILOT_DOTTED_KEY = "gameplay.pilot";
constexpr std::string_view PLAYER_PILOT_RULE_TEXT =
    "a player pilots a body of side player";

/** @brief The text a refused missile heading is told by. */
constexpr std::string_view MISSILE_HEADING_RULE_TEXT =
    "a missile flies towards the edge that removes it";

}  // namespace rtype::game::testing
