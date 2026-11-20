#pragma once

#include <cstddef>
#include <string_view>
#include "PlayBand.hpp"

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

/** @brief The levels of the resource fosse the repository ships, one map
 * each: space, two stages in open space, and fosse, three stages in the Pit;
 * then the names of their maps. */
constexpr std::string_view FOSSE_RESOURCE = "fosse";
constexpr std::string_view FOSSE_LEVEL = "fosse";
constexpr std::size_t STAGES_OF_THE_FOSSE_LEVEL = 3;
constexpr std::string_view FOSSE_MAP = "THE PIT";
constexpr std::string_view SPACE_LEVEL = "space";
constexpr std::size_t STAGES_OF_THE_SPACE_LEVEL = 2;
constexpr std::string_view SPACE_MAP = "OPEN SPACE";

/** @brief A script that registers a level as the map THE CAVES, and that
 * name. */
constexpr std::string_view REGISTERS_A_MAP = R"(
level.register("caves", { function() end }, { map = "THE CAVES" })
)";
constexpr std::string_view CAVES_MAP = "THE CAVES";

/** @brief A script that opens a level in the biome caves, and one that names
 * a biome that is no biome name. */
constexpr std::string_view REGISTERS_AN_OPENING_BIOME = R"(
level.register("caves", { function() end }, { biome = "caves" })
)";
constexpr std::string_view CAVES_BIOME_NAME = "caves";
constexpr std::string_view REGISTERS_A_WRONG_OPENING_BIOME = R"(
level.register("caves", { function() end }, { biome = "Deep Caves" })
)";

/** @brief A script that registers the map THE CAVES with the preview
 * the_caves; one that gives a preview that is no content name; and one that
 * gives a preview to a level that is no map. */
constexpr std::string_view REGISTERS_A_MAP_WITH_A_PREVIEW = R"(
level.register("caves", { function() end }, { map = "THE CAVES", preview = "the_caves" })
)";
constexpr std::string_view CAVES_PREVIEW = "the_caves";
constexpr std::string_view REGISTERS_A_PREVIEW_OUTSIDE_ITS_FOLDER = R"(
level.register("caves", { function() end }, { map = "THE CAVES", preview = "../caves" })
)";
constexpr std::string_view REGISTERS_A_PREVIEW_WITHOUT_A_MAP = R"(
level.register("caves", { function() end }, { preview = "the_caves" })
)";

/** @brief Scripts that register a level under a map name written in small
 * letters, seventeen characters long (one past the longest), and a number. */
constexpr std::string_view REGISTERS_A_MAP_IN_SMALL_LETTERS = R"(
level.register("caves", { function() end }, { map = "the caves" })
)";
constexpr std::string_view REGISTERS_A_MAP_NAME_TOO_LONG = R"(
level.register("caves", { function() end }, { map = "THE DEEPEST CAVES" })
)";
constexpr std::string_view REGISTERS_A_MAP_THAT_IS_NO_TEXT = R"(
level.register("caves", { function() end }, { map = 42 })
)";

/** @brief The band the level fosse declares for its biome: the ceiling and
 * the floor its maps leave free, rows 192 and 848. */
constexpr std::string_view FOSSE_BIOME = "fosse";
constexpr rtype::game::PlayBand FOSSE_BAND{.top = 192.0F, .bottom = 848.0F};

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

/** @brief A script that closes the biome pit with a ceiling on row 200 and a
 * floor on row 800, and the band it declares. */
constexpr std::string_view REGISTERS_A_BAND = R"(
level.register("caves", { function() end },
  { bands = { pit = { top = 200, bottom = 800 } } })
)";
constexpr std::string_view BANDED_BIOME = "pit";
constexpr rtype::game::PlayBand DECLARED_BAND{.top = 200.0F, .bottom = 800.0F};

/** @brief Scripts whose band is 40 units tall, less than a ship; whose floor
 * lies on row 1000, below the play area (984); and whose biome is no biome
 * name. */
constexpr std::string_view REGISTERS_A_BAND_NARROWER_THAN_A_SHIP = R"(
level.register("caves", { function() end },
  { bands = { pit = { top = 500, bottom = 540 } } })
)";
constexpr std::string_view REGISTERS_A_BAND_BELOW_THE_PLAY_AREA = R"(
level.register("caves", { function() end },
  { bands = { pit = { top = 200, bottom = 1000 } } })
)";
constexpr std::string_view REGISTERS_A_BAND_OF_NO_BIOME_NAME = R"(
level.register("caves", { function() end },
  { bands = { Pit = { top = 200, bottom = 800 } } })
)";

/** @brief A script that declares a start for the first stage of its level,
 * which always starts at the start of the level. */
constexpr std::string_view REGISTERS_A_START_FOR_THE_FIRST_STAGE = R"(
level.register("caves", { function() end, function() end },
  { starts = { [1] = { scroll = 0 } } })
)";

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
