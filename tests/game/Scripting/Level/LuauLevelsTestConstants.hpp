#pragma once

#include <cstddef>
#include <string_view>

/** @brief A script that registers two levels, of one and of two stages, and
 * their names in alphabetical order. */
constexpr std::string_view REGISTERS_TWO_LEVELS = R"(
level.register("wreck", { function() end })
level.register("caves", { function() end, function() end })
)";
constexpr std::string_view CAVES_LEVEL = "caves";
constexpr std::string_view WRECK_LEVEL = "wreck";
constexpr std::size_t STAGES_OF_CAVES = 2;

/** @brief A level no script registers. */
constexpr std::string_view UNKNOWN_LEVEL = "nowhere";

/** @brief A script that registers the same level twice. */
constexpr std::string_view REGISTERS_A_LEVEL_TWICE = R"(
level.register("wreck", { function() end })
level.register("wreck", { function() end })
)";

/** @brief Scripts that register a level without a name, without a stage and
 * with a stage that is no function. */
constexpr std::string_view REGISTERS_A_LEVEL_WITHOUT_NAME =
    R"(level.register("", { function() end }))";
constexpr std::string_view REGISTERS_A_LEVEL_WITHOUT_STAGE =
    R"(level.register("wreck", {}))";
constexpr std::string_view REGISTERS_A_STAGE_THAT_IS_NO_FUNCTION =
    R"(level.register("wreck", { function() end, 42 }))";

/** @brief Scripts that reach the level while they are first read, outside
 * any game. */
constexpr std::string_view WAITS_WHILE_READ = "level.wait(50)";
constexpr std::string_view READS_THE_SCROLL_WHILE_READ = "level.scroll()";
constexpr std::string_view COUNTS_THE_ENEMIES_WHILE_READ = "level.enemies()";
