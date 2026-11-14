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

/** @brief The resource of the level the repository ships, the name that
 * level is registered under, and its stages: one, then one more after its
 * checkpoint. */
constexpr std::string_view SHIPPED_LEVEL_RESOURCE = "training-run";
constexpr std::string_view SHIPPED_LEVEL = "training_run";
constexpr std::size_t STAGES_OF_THE_SHIPPED_LEVEL = 2;

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

/** @brief A script that registers a level with the LDtk file that places its
 * enemies, and that file. */
constexpr std::string_view REGISTERS_A_LEVEL_WITH_AN_LDTK_FILE = R"(
level.register("caves", { function() end }, { ldtk = "levels/caves.ldtk" })
)";
constexpr std::string_view CAVES_LDTK_FILE = "levels/caves.ldtk";

/** @brief The LDtk file of the level the repository ships. */
constexpr std::string_view SHIPPED_LEVEL_LDTK_FILE = "levels/training_run.ldtk";

/** @brief Scripts whose LDtk file leaves its content folder, is absolute, or
 * is no text, and one with an option no level has. */
constexpr std::string_view REGISTERS_AN_LDTK_FILE_ABOVE_THE_CONTENT = R"(
level.register("caves", { function() end }, { ldtk = "../caves.ldtk" })
)";
constexpr std::string_view REGISTERS_AN_ABSOLUTE_LDTK_FILE = R"(
level.register("caves", { function() end }, { ldtk = "/tmp/caves.ldtk" })
)";
constexpr std::string_view REGISTERS_AN_LDTK_FILE_THAT_IS_NO_TEXT = R"(
level.register("caves", { function() end }, { ldtk = 42 })
)";
constexpr std::string_view REGISTERS_AN_UNKNOWN_OPTION = R"(
level.register("caves", { function() end }, { file = "levels/caves.ldtk" })
)";

/** @brief Scripts that reach the placements while they are first read,
 * outside any game. */
constexpr std::string_view READS_THE_PLACEMENTS_WHILE_READ =
    "level.placements()";
constexpr std::string_view SPAWNS_THE_PLACED_WHILE_READ =
    "level.spawn_placed()";
