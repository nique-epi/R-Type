#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief The first game port and link token the registry tests give: game n
 * has port FIRST_GAME_PORT + n and token FIRST_LINK_TOKEN + n. */
constexpr std::uint16_t FIRST_GAME_PORT = 50000;
constexpr std::uint64_t FIRST_LINK_TOKEN = 0x7000;

/** @brief A token no game holds. */
constexpr std::uint64_t UNKNOWN_LINK_TOKEN = 0x9999;

/** @brief The requirement's limits: eight games at once, ten seconds to start,
 * three to stop. */
constexpr std::size_t EIGHT_GAMES = 8;
constexpr rtype::engine::Duration TEN_SECONDS_TO_START =
    std::chrono::seconds(10);
constexpr rtype::engine::Duration JUST_UNDER_TEN_SECONDS_TO_START =
    std::chrono::milliseconds(9999);
constexpr rtype::engine::Duration THREE_SECONDS_TO_STOP =
    std::chrono::seconds(3);

/** @brief The first two places of a supervisor, which the master numbers its
 * games by. */
constexpr std::uint16_t FIRST_PLACE = 1;
constexpr std::uint16_t SECOND_PLACE = 2;

/** @brief Three games, so the number the fourth one opened gets. */
constexpr std::uint32_t THREE_GAMES = 3;

/** @brief The sequences of the statuses the tests send. */
constexpr std::uint16_t SEQUENCE_FIVE = 5;
constexpr std::uint16_t SEQUENCE_FOUR = 4;
constexpr std::uint16_t LAST_SEQUENCE = 65535;
constexpr std::uint16_t FIRST_SEQUENCE = 0;

/** @brief The nicknames of the players a game reports, in the order of their
 * numbers: four, the seats of a game. */
constexpr std::array<std::string_view, 4> SUPERVISED_NICKNAMES{
    "PILOT1", "PILOT2", "PILOT3", "PILOT4"};

/** @brief The exit code of a game process that crashed. */
constexpr int CRASH_EXIT_CODE = 11;

/** @brief A player count above what a game holds, as a byte of a payload. */
constexpr unsigned char FIVE_PLAYERS_BYTE = 5;

/** @brief The program that runs a game in the tests, and the folder the games
 * write to: names, since no process starts. */
constexpr std::string_view PROGRAM_FOR_TESTS = "some/r-type_server";
constexpr std::string_view FOLDER_FOR_TESTS = "some/folder";

/** @brief The address and the first port of the players of the supervisor
 * tests: eight games need more players than SentDatagrams names. */
constexpr std::string_view SUPERVISED_PLAYER_ADDRESS = "192.0.2.50";
constexpr std::uint16_t FIRST_SUPERVISED_PLAYER_PORT = 40000;

/** @brief The file of commands of the games of a supervisor on port 4242,
 * which RecordingNetworkModule listens on. */
constexpr std::string_view GAMES_FILE_OF_PORT_4242 = "r-type_games_4242.cfg";

/** @brief Lines of a configuration that belong to the games, and the text the
 * file of the games holds once they are written, one per line in their order.
 */
constexpr std::string_view GAME_SEED_LINE = "set seed 7";
constexpr std::string_view GAME_PLUGIN_LINE = "plugin game-statistics";
constexpr std::string_view GAME_LINES_TEXT =
    "set seed 7\nplugin game-statistics\n";

/** @brief A line of the supervisor that sets its database, and the file it
 * names. */
constexpr std::string_view SET_DATABASE_LINE =
    "set database some/leaderboard.db";
constexpr std::string_view DATABASE_OF_THE_SUPERVISOR = "some/leaderboard.db";
