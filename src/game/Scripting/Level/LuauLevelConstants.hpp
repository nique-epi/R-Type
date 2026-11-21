#pragma once

#include <array>
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
constexpr std::string_view LEVEL_BOSSES_FUNCTION_NAME = "bosses";
constexpr std::string_view LEVEL_PLACEMENTS_FUNCTION_NAME = "placements";
constexpr std::string_view LEVEL_SPAWN_PLACED_FUNCTION_NAME = "spawn_placed";
constexpr std::string_view LEVEL_TRANSITION_FUNCTION_NAME = "transition";
constexpr std::string_view LEVEL_GROUND_FUNCTION_NAME = "ground";
constexpr std::string_view LEVEL_CEILING_FUNCTION_NAME = "ceiling";

/** @brief Where the functions of `level` take their arguments. */
constexpr int LEVEL_NAME_ARGUMENT = 1;
constexpr int LEVEL_STAGES_ARGUMENT = 2;
constexpr int LEVEL_WAIT_ARGUMENT = 1;
constexpr int LEVEL_BIOME_ARGUMENT = 1;
constexpr int LEVEL_GROUND_ARGUMENT = 1;

/** @brief The third argument of `level.register`, a table of options, and its
 * only key: the LDtk file of the level, relative to a content folder. */
constexpr int LEVEL_OPTIONS_ARGUMENT = 3;
constexpr const char* LEVEL_LDTK_KEY = "ldtk";

/** @brief The option that declares where some stages start, and the keys of
 * each start: the scroll, the biome and the scroll that biome began at. */
constexpr const char* LEVEL_STARTS_KEY = "starts";
constexpr const char* START_SCROLL_KEY = "scroll";

/** @brief The first stage a start may be declared for, counted from 1: the
 * first stage always starts at the start of the level. */
constexpr double FIRST_STARTED_STAGE_NUMBER = 2.0;
constexpr const char* START_BIOME_KEY = "biome";
constexpr const char* START_BIOME_SINCE_KEY = "biome_since";

/** @brief The option that closes the play area of some biomes with a ceiling
 * and a floor, and the keys of each band: the rows a ship flies between, in
 * logical units from the top edge of the playfield. */
constexpr const char* LEVEL_BANDS_KEY = "bands";

constexpr const char* BAND_TOP_KEY = "top";
constexpr const char* BAND_BOTTOM_KEY = "bottom";

/** @brief The option that makes a level a map the players of a waiting room
 * vote for, and the rules of its name, which the room shows: 1 to
 * MAXIMUM_MAP_NAME_SIZE characters, each a capital letter, a digit or a space,
 * so it is also a name the protocol carries. */
constexpr const char* LEVEL_MAP_KEY = "map";
constexpr std::size_t MAXIMUM_MAP_NAME_SIZE = 16;
constexpr std::string_view MAP_NAME_CHARACTERS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";

/** @brief The option that names the biome a level opens in: its players start
 * inside it, as if it began long before, and see no entrance. */
constexpr const char* LEVEL_OPENING_BIOME_KEY = "biome";

/** @brief The option that names the preview a map shows in the waiting room:
 * a content name, the file identifiers::MAP_PREVIEW_FOLDER/<name>.png of its
 * resource. */
constexpr const char* LEVEL_MAP_PREVIEW_KEY = "preview";

/** @brief Every option of a level; any other key is refused. */
constexpr std::array<std::string_view, 6> LEVEL_OPTION_KEYS{
    LEVEL_LDTK_KEY, LEVEL_STARTS_KEY,        LEVEL_BANDS_KEY,
    LEVEL_MAP_KEY,  LEVEL_OPENING_BIOME_KEY, LEVEL_MAP_PREVIEW_KEY};

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
    "level.wait, level.wait_scroll and level.transition only run in a stage "
    "of a level";
constexpr const char* WRONG_LEVEL_WAIT_MESSAGE =
    "a wait is a finite number of milliseconds, from 0";
constexpr const char* WRONG_LEVEL_TRANSITION_MESSAGE =
    "a transition lasts 3000 to 60000 milliseconds";
constexpr const char* WRONG_SCROLL_POSITION_MESSAGE =
    "a scroll position is a finite number of logical units, from 0";
constexpr const char* NO_LEVEL_PLAYED_MESSAGE =
    "the level is only reached during a game that plays one";
constexpr const char* WRONG_BIOME_NAME_MESSAGE =
    "the name of a biome is 1 to 24 lowercase letters, digits and underscores";
constexpr const char* LEVEL_YIELD_MESSAGE =
    "a stage may only yield through level.wait, level.wait_scroll and "
    "level.transition";
constexpr const char* WRONG_LEVEL_OPTIONS_MESSAGE =
    "the options of a level are a table whose keys are ldtk, starts, bands, "
    "map, biome and preview";
constexpr const char* WRONG_LEVEL_STARTS_MESSAGE =
    "the starts of a level map a stage from 2 to its last to a table of a "
    "scroll, a biome and a biome_since, finite numbers from 0 and a biome name";
constexpr const char* WRONG_LEVEL_MAP_MESSAGE =
    "the map of a level is the name the waiting room shows: 1 to 16 capital "
    "letters, digits and spaces";
constexpr const char* WRONG_LEVEL_PREVIEW_MESSAGE =
    "the preview of a map is the name of its image in the previews folder of "
    "its resource, without .png: 1 to 24 lowercase letters, digits and "
    "underscores, and only on a level given a map";
constexpr const char* WRONG_LEVEL_BANDS_MESSAGE =
    "the bands of a level map a biome name to a table of a top and a bottom, "
    "rows from 0 to the bottom of the play area, at least a ship apart";
constexpr const char* WRONG_LDTK_FILE_MESSAGE =
    "the ldtk option of a level is a relative path inside its resource, "
    "without ..";

}  // namespace rtype::game
