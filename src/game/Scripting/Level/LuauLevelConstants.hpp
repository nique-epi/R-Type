#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game {

/** @brief The table a script writes and directs a level through, and its
 * functions: `level.register(name, stages)`, `level.wait(ms)`,
 * `level.wait_scroll(position)`, `level.scroll()`, `level.pause_scroll()`,
 * `level.resume_scroll()`, `level.biome(name)`, `level.enemies()` and
 * `level.boss(name, options)`. */
constexpr std::string_view LEVEL_TABLE = "level";
constexpr std::string_view LEVEL_REGISTER_FUNCTION_NAME = "register";
constexpr std::string_view LEVEL_WAIT_FUNCTION_NAME = "wait";
constexpr std::string_view LEVEL_WAIT_SCROLL_FUNCTION_NAME = "wait_scroll";
constexpr std::string_view LEVEL_SCROLL_FUNCTION_NAME = "scroll";
constexpr std::string_view LEVEL_PAUSE_SCROLL_FUNCTION_NAME = "pause_scroll";
constexpr std::string_view LEVEL_RESUME_SCROLL_FUNCTION_NAME = "resume_scroll";
constexpr std::string_view LEVEL_BIOME_FUNCTION_NAME = "biome";
constexpr std::string_view LEVEL_ENEMIES_FUNCTION_NAME = "enemies";
constexpr std::string_view LEVEL_BOSS_FUNCTION_NAME = "boss";

/** @brief Where the functions of `level` take their arguments. */
constexpr int LEVEL_NAME_ARGUMENT = 1;
constexpr int LEVEL_STAGES_ARGUMENT = 2;
constexpr int LEVEL_WAIT_ARGUMENT = 1;
constexpr int LEVEL_BIOME_ARGUMENT = 1;
constexpr int LEVEL_BOSS_ARGUMENT = 1;
constexpr int LEVEL_BOSS_OPTIONS_ARGUMENT = 2;

/** @brief The option of `level.boss` that gives the health of the boss, in
 * percent of its sheet, and its smallest value; the largest is the whole
 * health. */
constexpr const char* BOSS_HEALTH_PERCENT_KEY = "health_percent";
constexpr double MINIMUM_BOSS_HEALTH_PERCENT = 1.0;

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
    "level.wait, level.wait_scroll and level.boss only run in a stage of a "
    "level";
constexpr const char* WRONG_LEVEL_WAIT_MESSAGE =
    "a wait is a finite number of milliseconds, from 0";
constexpr const char* WRONG_SCROLL_POSITION_MESSAGE =
    "a scroll position is a finite number of logical units, from 0";
constexpr const char* NO_LEVEL_PLAYED_MESSAGE =
    "the level is only reached during a game that plays one";
constexpr const char* WRONG_BIOME_NAME_MESSAGE =
    "the name of a biome is 1 to 24 lowercase letters, digits and underscores";
constexpr const char* LEVEL_YIELD_MESSAGE =
    "a stage may only yield through level.wait, level.wait_scroll and "
    "level.boss";
constexpr const char* NOT_A_BOSS_MESSAGE =
    "level.boss names the sheet of a boss, one with a [gameplay.boss] table";
constexpr const char* WRONG_BOSS_HEALTH_PERCENT_MESSAGE =
    "the health_percent of a boss is a whole number from 1 to 100";

}  // namespace rtype::game
