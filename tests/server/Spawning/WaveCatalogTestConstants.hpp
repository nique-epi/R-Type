#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include <utility>
#include "BydoKind.hpp"
#include "BydoWave.hpp"
#include "WaveCurve.hpp"

namespace rtype::server::testing {

/** @brief The name the tests give the waves file, which the errors show. */
constexpr std::string_view TEST_FILE_NAME = "test.toml";

/**
 * @brief The difficulty calm, written by the fixtures as its header on line 1
 * and these seven lines, in this order, on lines 2 to 8. The keys and the
 * enemy names are written out, not taken from the code: a key or a name
 * renamed in the code breaks every file already written, so the tests must
 * turn red.
 */
constexpr std::string_view CALM_HEADER = "[difficulty.calm]";
constexpr std::array<std::pair<std::string_view, std::string_view>, 7>
    CALM_VALUES{{{"first_wave_bydos", "2"},
                 {"bydos_added_per_wave", "1"},
                 {"first_spawn_interval_ms", "2500"},
                 {"spawn_interval_cut_per_wave_ms", "250"},
                 {"shortest_spawn_interval_ms", "1500"},
                 {"wave_pause_ms", "5000"},
                 {"enemies", R"(["drifter", "weaver"])"}}};

/**
 * @brief How the waves of calm grow, and its enemies, as the lines give them.
 */
constexpr game::WaveCurve CALM_CURVE{
    .firstWave =
        game::BydoWave{.bydoCount = 2,
                       .spawnInterval = std::chrono::milliseconds(2500)},
    .bydosAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::milliseconds(1500),
    .wavePause = std::chrono::milliseconds(5000)};
constexpr std::array CALM_ENEMIES{game::BydoKind::Drifter,
                                  game::BydoKind::Weaver};

/** @brief A difficulty storm, written before calm in a file, and its waves. */
constexpr std::string_view STORM_TABLE = R"([difficulty.storm]
first_wave_bydos = 4
bydos_added_per_wave = 2
first_spawn_interval_ms = 1500
spawn_interval_cut_per_wave_ms = 250
shortest_spawn_interval_ms = 500
wave_pause_ms = 3000
enemies = ["weaver", "drifter", "weaver"]
)";
constexpr game::WaveCurve STORM_CURVE{
    .firstWave =
        game::BydoWave{.bydoCount = 4,
                       .spawnInterval = std::chrono::milliseconds(1500)},
    .bydosAddedPerWave = 2,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::milliseconds(500),
    .wavePause = std::chrono::milliseconds(3000)};
constexpr std::array STORM_ENEMIES{
    game::BydoKind::Weaver, game::BydoKind::Drifter, game::BydoKind::Weaver};

/**
 * @brief Files that do not describe difficulties: an empty one, and one whose
 * difficulty calm is a number.
 */
constexpr std::string_view EMPTY_FILE;
constexpr std::string_view LEVEL_THAT_IS_NOT_A_TABLE =
    "[difficulty]\ncalm = 3\n";

/**
 * @brief Values that break a line of calm: a number with text after it, which
 * is not TOML, a fraction, and a float that is whole but still not a TOML
 * integer.
 */
constexpr std::string_view NOT_A_TOML_VALUE = "2500 ms";
constexpr std::string_view FRACTION = "2500.5";
constexpr std::string_view WHOLE_FLOAT = "2500.0";

/** @brief A misspelt key, which the fixtures add as line 9 of calm. */
constexpr std::string_view MISSPELT_KEY = "frist_wave_bydos";

/** @brief The bounds of the Bydos of a first wave, and one past each. */
constexpr std::string_view NO_BYDO = "0";
constexpr std::string_view FEWEST_BYDOS = "1";
constexpr std::string_view MOST_BYDOS = "100";
constexpr std::string_view ONE_BYDO_TOO_MANY = "101";

/**
 * @brief Shortest times of calm, whose first is 2500 ms: equal to it, and one
 * millisecond longer.
 */
constexpr std::string_view SHORTEST_EQUAL_TO_THE_FIRST = "2500";
constexpr std::string_view SHORTEST_LONGER_THAN_THE_FIRST = "2501";

/**
 * @brief Enemies that break the last line of calm: a name no kind has, an
 * empty list, a name that is not in a list, and a list of something else than
 * names.
 */
constexpr std::string_view UNKNOWN_ENEMY = R"(["drifter", "bos"])";
constexpr std::string_view NO_ENEMY = "[]";
constexpr std::string_view ENEMY_OUTSIDE_A_LIST = R"("drifter")";
constexpr std::string_view NUMBER_AS_AN_ENEMY = "[1]";

/**
 * @brief A waves file no test writes, and the shipped one, from the assets
 * folder.
 */
constexpr std::string_view MISSING_FILE = "no-such-folder/waves.toml";
constexpr std::string_view SHIPPED_FILE_IN_ASSETS = "waves/waves.toml";

/**
 * @brief What the errors say. A value of `first_wave_bydos = ` starts on
 * column 20, the second name of `enemies = ["drifter", ` on column 23, the key
 * of line 9 on column 1.
 */
constexpr std::string_view UNKNOWN_DIFFICULTY_MESSAGE =
    "no difficulty normal in the waves file, which describes: calm, storm";
constexpr std::string_view NO_BYDO_MESSAGE =
    "test.toml:2:20: difficulty.calm.first_wave_bydos must be an integer from "
    "1 to 100";
constexpr std::string_view MISSPELT_KEY_MESSAGE =
    "test.toml:9:1: unknown key difficulty.calm.frist_wave_bydos";
constexpr std::string_view UNKNOWN_ENEMY_MESSAGE =
    R"(test.toml:8:23: difficulty.calm.enemies: unknown enemy "bos"; known: drifter, weaver)";
constexpr std::string_view MISSING_PAUSE_TEXT =
    "difficulty.calm.wave_pause_ms is missing";
constexpr std::string_view MISSING_DIFFICULTIES_TEXT = "difficulty is missing";
constexpr std::string_view NOT_A_TABLE_TEXT = "difficulty.calm must be a table";
constexpr std::string_view LONGER_THAN_THE_FIRST_TEXT =
    "difficulty.calm.shortest_spawn_interval_ms must be an integer from 1 to "
    "2500";
constexpr std::string_view INVALID_ENEMY_LIST_TEXT =
    "difficulty.calm.enemies must be a list of 1 to 100 enemy names";
constexpr std::string_view SYNTAX_ERROR_START = "test.toml:4:";
constexpr std::string_view UNREADABLE_FILE_START =
    "cannot read the waves file ";

}  // namespace rtype::server::testing
