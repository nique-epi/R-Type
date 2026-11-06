#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "EnemyKilled.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "TicksToReach.hpp"
#include "TimeTestConstants.hpp"

/** @brief The name the plugin is loaded by, the setting that names its file,
 * and the file the tests have it write. */
constexpr std::string_view GAME_STATISTICS_PLUGIN_NAME = "game-statistics";
constexpr std::string_view STATISTICS_FILE_SETTING = "game-statistics-file";
constexpr std::string_view STATISTICS_FILE_NAME = "statistics.csv";

/** @brief STATISTICS_FILE_NAME cut where a game puts its port: its name,
 * what goes before the port, and its extension. */
constexpr std::string_view STATISTICS_FILE_STEM = "statistics";
constexpr std::string_view STATISTICS_PORT_SEPARATOR = "_";
constexpr std::string_view STATISTICS_FILE_EXTENSION = ".csv";

/** @brief The file the plugin writes for STATISTICS_FILE_NAME on
 * STATISTICS_TEST_PORT: the name, the port, then the extension. */
constexpr std::string_view WRITTEN_STATISTICS_FILE_NAME = "statistics_4307.csv";

/** @brief The games the tests drive, and the port the network double gives. */
constexpr std::size_t GAMES_IN_THE_TESTS = 2;
constexpr std::uint16_t STATISTICS_TEST_PORT = 4307;

/** @brief The file the plugin writes, next to the executable, without a
 * setting, for STATISTICS_TEST_PORT. */
constexpr std::string_view DEFAULT_STATISTICS_FILE_NAME =
    "r-type_statistics_4307.csv";

/** @brief A game moving into its match, and out of it at its end. */
constexpr rtype::server::GameStateChanged GAME_STARTED{
    .previous = rtype::server::GameState::Starting,
    .current = rtype::server::GameState::InProgress};
constexpr rtype::server::GameStateChanged GAME_FINISHED{
    .previous = rtype::server::GameState::InProgress,
    .current = rtype::server::GameState::Finished};

/** @brief A Bydo killed by the first pilot, and one by the second. */
constexpr rtype::game::EnemyKilled KILL_BY_FIRST_PILOT{.playerNumber = 1,
                                                       .points = 100};
constexpr rtype::game::EnemyKilled KILL_BY_SECOND_PILOT{.playerNumber = 2,
                                                        .points = 300};

/** @brief The lines the file must hold, written from the format the issue
 * asks for: the header, the kill of each pilot in game 1 and of the first in
 * game 2, the total of game 1 with both kills and without any. */
constexpr std::string_view STATISTICS_HEADER_LINE =
    "event,game,player,points\n";
constexpr std::string_view FIRST_PILOT_KILL_IN_GAME_ONE = "kill,1,1,100\n";
constexpr std::string_view SECOND_PILOT_KILL_IN_GAME_ONE = "kill,1,2,300\n";
constexpr std::string_view FIRST_PILOT_KILL_IN_GAME_TWO = "kill,2,1,100\n";
constexpr std::string_view TOTAL_OF_GAME_ONE = "total,1,,400\n";
constexpr std::string_view EMPTY_TOTAL_OF_GAME_ONE = "total,1,,0\n";

/** @brief The tick that reaches the second the rows wait, and the ticks short
 * of it: a tick lasts a little under a sixtieth of a second, so the second is
 * reached on the 61st. */
constexpr std::size_t TICKS_TO_A_WRITE =
    rtype::engine::testing::ticksToReach(rtype::engine::testing::ONE_SECOND);
constexpr std::size_t TICKS_SHORT_OF_A_WRITE = TICKS_TO_A_WRITE - 1;
