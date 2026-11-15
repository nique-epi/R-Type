#pragma once

#include <cstddef>
#include <string_view>

/** @brief The content folders of these tests, below a temporary folder, and
 * the level their scripts register, with its LDtk file. */
constexpr std::string_view FIRST_CONTENT = "first";
constexpr std::string_view SECOND_CONTENT = "second";
constexpr std::string_view PLACED_LEVEL = "placed";
constexpr std::string_view PLACED_LEVEL_SCRIPT = "server/placed.luau";
constexpr std::string_view PLACED_LEVEL_LDTK = "levels/placed.ldtk";
constexpr std::string_view REGISTERS_THE_PLACED_LEVEL = R"(
level.register("placed", { function() end }, { ldtk = "levels/placed.ldtk" })
)";

/** @brief A Drifter of 24 by 24 pixels at the top left corner of its level:
 * its center is at 48, 48. */
constexpr std::string_view DRIFTER_AT_THE_CORNER = R"json(
  {"__identifier": "Drifter", "px": [0, 0], "__pivot": [0, 0],
   "width": 24, "height": 24, "fieldInstances": []}
)json";
constexpr float CORNER_CENTER = 48.0F;

/** @brief The same Drifter 100 pixels to the right: its center is at x 448. */
constexpr std::string_view DRIFTER_FURTHER_RIGHT = R"json(
  {"__identifier": "Drifter", "px": [100, 0], "__pivot": [0, 0],
   "width": 24, "height": 24, "fieldInstances": []}
)json";
constexpr float FURTHER_RIGHT_CENTER_X = 448.0F;

/** @brief An entity Ghost, which names no sheet, at the top left corner, and
 * what the error says of it. */
constexpr std::string_view GHOST_AT_THE_CORNER = R"json(
  {"__identifier": "Ghost", "px": [0, 0], "__pivot": [0, 0],
   "width": 24, "height": 24, "fieldInstances": []}
)json";
constexpr std::string_view GHOST_REFUSAL =
    "the entity Ghost placed at 48, 48 names no sheet ghost";

/** @brief A Ship, the sheet of the players. */
constexpr std::string_view SHIP_AT_THE_CORNER = R"json(
  {"__identifier": "Ship", "px": [0, 0], "__pivot": [0, 0],
   "width": 32, "height": 16, "fieldInstances": []}
)json";

/** @brief How many enemies the LDtk file of the level the repository ships
 * places: three Drifters, two Weavers and a Reaper in its first stage, four
 * Drifters, two Voltjellies, a Weaver and two Reapers in its second. */
constexpr std::size_t ENEMIES_OF_THE_SHIPPED_LEVEL = 15;
