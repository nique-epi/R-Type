#pragma once

#include <array>
#include <string_view>

/** @brief The file the tests write the project to, below their folder. */
constexpr std::string_view GROUND_LDTK_FILE = "ground.ldtk";

/** @brief A project of two levels, 2 then 1 pixel wide, each giving its floor
 * and its ceiling; the second column of the first level has no ceiling. Laid
 * end to end, their floors are 220, 224 and 230, their ceilings 40, none and
 * 44. */
constexpr std::string_view TWO_LEVELS_WITH_GROUND = R"json({"levels": [
  {"identifier": "Level_1", "pxWid": 2, "fieldInstances": [
    {"__identifier": "Floor", "__type": "Array<Int>", "__value": [220, 224]},
    {"__identifier": "Ceiling", "__type": "Array<Int>", "__value": [40, -1]}]},
  {"identifier": "Level_2", "pxWid": 1, "fieldInstances": [
    {"__identifier": "Floor", "__type": "Array<Int>", "__value": [230]},
    {"__identifier": "Ceiling", "__type": "Array<Int>", "__value": [44]}]}
]})json";
constexpr std::array FLOORS_OF_THE_TWO_LEVELS{220, 224, 230};
constexpr std::array CEILINGS_OF_THE_TWO_LEVELS{40, -1, 44};

/** @brief A project exported before the ground existed: its level has no
 * field. */
constexpr std::string_view PROJECT_WITHOUT_GROUND = R"json({"levels": [
  {"identifier": "Level_1", "pxWid": 2, "fieldInstances": []}
]})json";

/** @brief A level 3 pixels wide whose floor gives 2 rows. */
constexpr std::string_view FLOOR_SHORTER_THAN_ITS_LEVEL = R"json({"levels": [
  {"identifier": "Level_1", "pxWid": 3, "fieldInstances": [
    {"__identifier": "Floor", "__type": "Array<Int>", "__value": [220, 224]},
    {"__identifier": "Ceiling", "__type": "Array<Int>", "__value": [40, 40, 40]}]}
]})json";

/** @brief A project whose first level gives its ground and whose second gives
 * no ceiling. */
constexpr std::string_view SECOND_LEVEL_WITHOUT_CEILING = R"json({"levels": [
  {"identifier": "Level_1", "pxWid": 1, "fieldInstances": [
    {"__identifier": "Floor", "__type": "Array<Int>", "__value": [220]},
    {"__identifier": "Ceiling", "__type": "Array<Int>", "__value": [40]}]},
  {"identifier": "Level_2", "pxWid": 1, "fieldInstances": [
    {"__identifier": "Floor", "__type": "Array<Int>", "__value": [220]}]}
]})json";
