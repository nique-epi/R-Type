#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game {

/** @brief The table a script writes and directs a level through, and its
 * functions: `level.register(name, stages)`, `level.wait(ms)`,
 * `level.wait_scroll(position)`, `level.scroll()`, `level.pause_scroll()`,
 * `level.resume_scroll()`, `level.biome(name)`, `level.enemies()`,
 * `level.placements()` and `level.spawn_placed()`. */
constexpr std::string_view LEVEL_TABLE = "level";
constexpr std::string_view LEVEL_REGISTER_FUNCTION_NAME = "register";
constexpr std::string_view LEVEL_WAIT_FUNCTION_NAME = "wait";
constexpr std::string_view LEVEL_WAIT_SCROLL_FUNCTION_NAME = "wait_scroll";
constexpr std::string_view LEVEL_SCROLL_FUNCTION_NAME = "scroll";
constexpr std::string_view LEVEL_PAUSE_SCROLL_FUNCTION_NAME = "pause_scroll";
constexpr std::string_view LEVEL_RESUME_SCROLL_FUNCTION_NAME = "resume_scroll";
constexpr std::string_view LEVEL_BIOME_FUNCTION_NAME = "biome";
constexpr std::string_view LEVEL_ENEMIES_FUNCTION_NAME = "enemies";
constexpr std::string_view LEVEL_PLACEMENTS_FUNCTION_NAME = "placements";
constexpr std::string_view LEVEL_SPAWN_PLACED_FUNCTION_NAME = "spawn_placed";

/** @brief Where the functions of `level` take their arguments. */
constexpr int LEVEL_NAME_ARGUMENT = 1;
constexpr int LEVEL_STAGES_ARGUMENT = 2;
constexpr int LEVEL_WAIT_ARGUMENT = 1;
constexpr int LEVEL_BIOME_ARGUMENT = 1;

/** @brief The third argument of `level.register`, a table of options, and its
 * only key: the LDtk file of the level, relative to a content folder. */
constexpr int LEVEL_OPTIONS_ARGUMENT = 3;
constexpr const char* LEVEL_LDTK_KEY = "ldtk";

/** @brief The part of a path that leaves its folder. */
constexpr std::string_view PARENT_FOLDER_PART = "..";

/** @brief The keys of a placement a script reads, besides the x and y of its
 * center (POINT_X_KEY, POINT_Y_KEY), and how many keys it has. */
constexpr const char* PLACEMENT_ENTITY_KEY = "entity";
constexpr const char* PLACEMENT_SHEET_KEY = "sheet";
constexpr const char* PLACEMENT_WIDTH_KEY = "width";
constexpr const char* PLACEMENT_FIELDS_KEY = "fields";
constexpr int PLACEMENT_KEYS = 6;

/** @brief How an error names a placed entity: `the entity Ghost placed at
 * 2224, 300 names no sheet ghost`. */
constexpr std::string_view PLACED_ENTITY_TEXT = "the entity ";
constexpr std::string_view PLACED_AT_TEXT = " placed at ";
constexpr std::string_view POINT_SEPARATOR = ", ";
constexpr std::string_view NO_SHEET_REASON = " names no sheet ";
constexpr std::string_view SHIP_SHEET_REASON =
    " names a ship, which no level may place";

/** @brief How many stages a level may hold: each one after the first starts
 * at a checkpoint. */
constexpr std::size_t MAXIMUM_LEVEL_STAGES = 64;

/** @brief What the functions of `level` refuse with, and what stops a level
 * whose stage yields by itself. */
constexpr const char* EMPTY_LEVEL_NAME_MESSAGE = "a level needs a name";
constexpr const char* DUPLICATE_LEVEL_MESSAGE =
    "a level of that name is already registered";
constexpr const char* WRONG_LEVEL_STAGES_MESSAGE =
    "the stages of a level are a list of 1 to 64 functions";
constexpr const char* LEVEL_WAIT_OUTSIDE_STAGE_MESSAGE =
    "level.wait and level.wait_scroll only run in a stage of a level";
constexpr const char* WRONG_LEVEL_WAIT_MESSAGE =
    "a wait is a finite number of milliseconds, from 0";
constexpr const char* WRONG_SCROLL_POSITION_MESSAGE =
    "a scroll position is a finite number of logical units, from 0";
constexpr const char* NO_LEVEL_PLAYED_MESSAGE =
    "the level is only reached during a game that plays one";
constexpr const char* WRONG_BIOME_NAME_MESSAGE =
    "the name of a biome is 1 to 24 lowercase letters, digits and underscores";
constexpr const char* LEVEL_YIELD_MESSAGE =
    "a stage may only yield through level.wait and level.wait_scroll";
constexpr const char* WRONG_LEVEL_OPTIONS_MESSAGE =
    "the options of a level are a table whose only key is ldtk";
constexpr const char* WRONG_LDTK_FILE_MESSAGE =
    "the ldtk option of a level is a relative path inside its resource, "
    "without ..";

}  // namespace rtype::game
