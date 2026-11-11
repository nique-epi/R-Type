#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game::testing {

/** @brief Pieces of `[gameplay]` about the collision layers, to put before
 * the tables of a sheet: an enemy that hits the ships and the walls, and
 * enemies whose layer or hits a reader refuses. */
constexpr std::string_view ENEMY_HITTING_SHIPS_AND_WALLS =
    "[gameplay]\nside = \"enemy\"\nlayer = \"enemy\"\n"
    "hits = [\"player\", \"wall\"]\npilot = \"straight\"\n";
constexpr std::string_view ENEMY_WITHOUT_LAYER =
    "[gameplay]\nside = \"enemy\"\npilot = \"straight\"\n";
constexpr std::string_view ENEMY_IN_A_LAYER_WITH_AN_UPPERCASE_LETTER =
    "[gameplay]\nside = \"enemy\"\nlayer = \"Enemy\"\npilot = \"straight\"\n";
constexpr std::string_view ENEMY_IN_A_LAYER_WITHOUT_NAME =
    "[gameplay]\nside = \"enemy\"\nlayer = \"\"\npilot = \"straight\"\n";
constexpr std::string_view ENEMY_WHOSE_HITS_ARE_ONE_TEXT =
    "[gameplay]\nside = \"enemy\"\nlayer = \"enemy\"\nhits = \"player\"\n"
    "pilot = \"straight\"\n";

/** @brief The layer and the hits those pieces name. */
constexpr std::string_view ENEMY_LAYER_NAME = "enemy";
constexpr std::string_view PLAYER_LAYER_NAME = "player";
constexpr std::string_view WALL_LAYER_NAME = "wall";

/** @brief What a reader answers a layer that is not named as a type is: the
 * layer is on line 3 of the sheet, its value on column 9. */
constexpr std::string_view LAYER_NAME_REFUSAL =
    "/sheets/test_sheet.toml:3:9: gameplay.layer must be 1 to 24 lowercase "
    "letters, digits or underscores";

/** @brief Three whole sheets of a catalog: a bug that hits nothing, a shot
 * that hits the walls and the bugs, and a rock, a wall that hits the shots.
 * In the order of their names the layers are bug, shot, wall. */
constexpr std::string_view BUG_SHEET_NAME = "bug";
constexpr std::string_view OTHER_BUG_SHEET_NAME = "other_bug";
constexpr std::string_view SHOT_SHEET_NAME = "shot";
constexpr std::string_view ROCK_SHEET_NAME = "rock";
constexpr std::string_view BUG_SHEET_TEXT = R"([gameplay]
side = "enemy"
layer = "bug"
pilot = "straight"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[appearance]
)";
constexpr std::string_view SHOT_SHEET_TEXT = R"([gameplay]
side = "player_missile"
layer = "shot"
hits = ["wall", "bug"]
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
constexpr std::string_view ROCK_SHEET_TEXT = R"([gameplay]
side = "enemy"
layer = "wall"
hits = ["shot"]
pilot = "straight"
[gameplay.hitbox]
width = 96
height = 96
[gameplay.motion]
kind = "steady"
top_speed = 240
[gameplay.behaviour]
heading = -1
[appearance]
)";

/** @brief The sets of the three layers, one bit each in the order of their
 * names, and what the shot hits: the walls and the bugs. */
constexpr std::uint32_t BUG_LAYER_SET = 0x1U;
constexpr std::uint32_t SHOT_LAYER_SET = 0x2U;
constexpr std::uint32_t WALL_LAYER_SET = 0x4U;
constexpr std::uint32_t WALLS_AND_BUGS_SET = 0x5U;
constexpr std::uint32_t NO_LAYER_SET = 0x0U;

/** @brief What a catalog answers the shot when no sheet lies in the layer of
 * the walls: `"wall"` is on line 4 of the shot, column 9. */
constexpr std::string_view UNKNOWN_HIT_REFUSAL =
    R"(shot.toml:4:9: gameplay.hits: unknown value "wall"; known: bug, shot)";

/** @brief A sheet of an enemy up to its layer, and from its pilot on: a test
 * puts a layer of its own between them. */
constexpr std::string_view SHEET_UP_TO_ITS_LAYER =
    "[gameplay]\nside = \"enemy\"\nlayer = \"";
constexpr std::string_view SHEET_FROM_ITS_PILOT =
    "\"\npilot = \"straight\"\n[gameplay.hitbox]\nwidth = 96\nheight = 96\n"
    "[gameplay.motion]\nkind = \"steady\"\ntop_speed = 240\n"
    "[gameplay.behaviour]\nheading = -1\n[appearance]\n";
constexpr std::string_view GENERATED_LAYER_PREFIX = "layer_";

/** @brief As many layers as a filter tells apart, and one more. */
constexpr std::size_t MOST_LAYERS_NAMED = 32;
constexpr std::size_t ONE_LAYER_TOO_MANY = 33;

}  // namespace rtype::game::testing
