#pragma once

#include <cstddef>
#include <string_view>

/** @brief The entities of a Mobs layer: a Lamprey at pixel 368, 32 (32 by 40,
 * pivot at the top left), then a BydoViolet at pixel 100, 100 (24 by 24):
 * read in the order of x, the BydoViolet comes first. */
constexpr std::string_view LAMPREY_THEN_BYDO_VIOLET = R"json(
  {"__identifier": "Lamprey", "px": [368, 32], "__pivot": [0, 0],
   "width": 32, "height": 40, "fieldInstances": []},
  {"__identifier": "BydoViolet", "px": [100, 100], "__pivot": [0, 0],
   "width": 24, "height": 24, "fieldInstances": []}
)json";
constexpr std::size_t TWO_ENEMIES = 2;
constexpr std::string_view LAMPREY_ENTITY = "Lamprey";
constexpr std::string_view LAMPREY_SHEET = "lamprey";
constexpr std::string_view BYDO_VIOLET_ENTITY = "BydoViolet";
constexpr std::string_view BYDO_VIOLET_SHEET = "bydo_violet";

/** @brief Where that Lamprey is in logical units: the center of its box,
 * pixel 384, 52, at 4 units a pixel, and its width. */
constexpr float LAMPREY_CENTER_X = 1536.0F;
constexpr float LAMPREY_CENTER_Y = 208.0F;
constexpr float LAMPREY_WIDTH = 128.0F;

/** @brief A Lamprey at the top left corner: its center is half its box in,
 * 16 by 20 pixels, 64 by 80 units. */
constexpr std::string_view LAMPREY_AT_THE_TOP_LEFT = R"json(
  {"__identifier": "Lamprey", "px": [0, 0], "__pivot": [0, 0],
   "width": 32, "height": 40, "fieldInstances": []}
)json";
constexpr float TOP_LEFT_CENTER_X = 64.0F;
constexpr float TOP_LEFT_CENTER_Y = 80.0F;

/** @brief A Lamprey against the right and bottom edges of a 480 by 270 level,
 * at pixel 448, 230: its center is at pixel 464, 250, 1856 by 1000 units. */
constexpr std::string_view LAMPREY_AT_THE_BOTTOM_RIGHT = R"json(
  {"__identifier": "Lamprey", "px": [448, 230], "__pivot": [0, 0],
   "width": 32, "height": 40, "fieldInstances": []}
)json";
constexpr float BOTTOM_RIGHT_CENTER_X = 1856.0F;
constexpr float BOTTOM_RIGHT_CENTER_Y = 1000.0F;

/** @brief A Lamprey whose pivot is its center, at pixel 100, 100: its center
 * is that pixel, 400 by 400 units. */
constexpr std::string_view LAMPREY_WITH_A_CENTERED_PIVOT = R"json(
  {"__identifier": "Lamprey", "px": [100, 100], "__pivot": [0.5, 0.5],
   "width": 32, "height": 40, "fieldInstances": []}
)json";
constexpr float CENTERED_PIVOT_CENTER = 400.0F;

/** @brief A project of two levels of 480 pixels, the second placing a Lamprey
 * at its top left corner: the second level starts 1920 units in, so the
 * center is at x 1984. */
constexpr std::string_view TWO_LEVELS_PROJECT = R"json({
  "levels": [
    {"identifier": "Level_1", "pxWid": 480, "pxHei": 270,
     "layerInstances": [
       {"__identifier": "Mobs", "__type": "Entities", "entityInstances": []}
     ]},
    {"identifier": "Level_2", "pxWid": 480, "pxHei": 270,
     "layerInstances": [
       {"__identifier": "Mobs", "__type": "Entities", "entityInstances": [
         {"__identifier": "Lamprey", "px": [0, 0], "__pivot": [0, 0],
          "width": 32, "height": 40, "fieldInstances": []}
       ]}
     ]}
  ]
})json";
constexpr float SECOND_LEVEL_CENTER_X = 1984.0F;

/** @brief An entity with a field of each kind the game reads, in this order,
 * and one whose value is null. */
constexpr std::string_view LAMPREY_WITH_FIELDS = R"json(
  {"__identifier": "Lamprey", "px": [0, 0], "__pivot": [0, 0],
   "width": 32, "height": 40, "fieldInstances": [
     {"__identifier": "count", "__type": "Int", "__value": 3},
     {"__identifier": "speed", "__type": "Float", "__value": 1.5},
     {"__identifier": "armed", "__type": "Bool", "__value": true},
     {"__identifier": "name", "__type": "String", "__value": "a"},
     {"__identifier": "side", "__type": "Enum(Side)", "__value": "left"},
     {"__identifier": "unset", "__type": "Int", "__value": null}
   ]}
)json";
constexpr std::size_t FIELDS_READ = 5;
constexpr double COUNT_FIELD = 3.0;
constexpr double SPEED_FIELD = 1.5;
constexpr std::string_view NAME_FIELD = "a";
constexpr std::string_view SIDE_FIELD = "left";
constexpr std::string_view COUNT_FIELD_NAME = "count";
constexpr std::string_view SIDE_FIELD_NAME = "side";

/** @brief An entity with a field of a kind the game does not read, a point,
 * and that field's name. */
constexpr std::string_view LAMPREY_WITH_A_POINT_FIELD = R"json(
  {"__identifier": "Lamprey", "px": [0, 0], "__pivot": [0, 0],
   "width": 32, "height": 40, "fieldInstances": [
     {"__identifier": "target", "__type": "Point",
      "__value": {"cx": 1, "cy": 2}}
   ]}
)json";
constexpr std::string_view POINT_FIELD_NAME = "target";

/** @brief An entity whose identifier, 25 letters long, names no sheet. */
constexpr std::string_view ENTITY_WITH_A_LONG_IDENTIFIER = R"json(
  {"__identifier": "Abcdefghijklmnopqrstuvwxy", "px": [0, 0],
   "__pivot": [0, 0], "width": 32, "height": 40, "fieldInstances": []}
)json";
constexpr std::string_view LONG_IDENTIFIER = "Abcdefghijklmnopqrstuvwxy";

/** @brief A project whose level has no Mobs layer, and that level's name. */
constexpr std::string_view NO_MOBS_LAYER_PROJECT = R"json({
  "levels": [{
    "identifier": "Level_1", "pxWid": 480, "pxHei": 270,
    "layerInstances": [{"__identifier": "Walls", "__type": "Tiles"}]
  }]
})json";
constexpr std::string_view FIRST_LEVEL = "Level_1";

/** @brief A project whose levels are saved in separate files. */
constexpr std::string_view SEPARATE_LEVELS_PROJECT = R"json({
  "levels": [{
    "identifier": "Level_1", "pxWid": 480, "pxHei": 270,
    "layerInstances": null, "externalRelPath": "test/Level_1.ldtkl"
  }]
})json";

/** @brief A text that is no JSON. */
constexpr std::string_view NOT_JSON = "not json";
