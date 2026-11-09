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

/** @brief How many times a client repeats its request in the tests of a new
 * game: once every 500 ms, for a game that starts. */
constexpr std::size_t REPEATED_REQUESTS = 3;

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

/** @brief What the nickname of each player of the tests starts with, before
 * its number from 1. */
constexpr std::string_view SUPERVISED_NICKNAME_PREFIX = "PILOT";

/** @brief The index of a fifth player, one more than the seats of a game. */
constexpr std::size_t FIFTH_PLAYER = 4;

/** @brief The requirement's wait for a player sent to a game: three seconds,
 * then its seat is free again. */
constexpr rtype::engine::Duration THREE_SECONDS_TO_SIT =
    std::chrono::seconds(3);
constexpr rtype::engine::Duration JUST_UNDER_THREE_SECONDS_TO_SIT =
    std::chrono::milliseconds(2999);

/** @brief The traffic file given to a supervisor, and the name of the one
 * game 3 writes from it, in the same folder. */
constexpr std::string_view TRAFFIC_FILE_OF_THE_SUPERVISOR =
    "some/folder/traffic.csv";
constexpr std::string_view TRAFFIC_FILE_OF_GAME_THREE = "traffic_game_3.csv";

/** @brief Options of a supervisor: a level, a simulated network, a traffic
 * file; and the options with which it writes no journal and nothing to
 * standard error. */
constexpr std::array<std::string_view, 6> SUPERVISOR_LEVEL_NETWORK_AND_TRAFFIC{
    "--log-level", "debug", "--network", "bad", "--traffic-csv", "traffic.csv"};
constexpr std::array<std::string_view, 2> SUPERVISOR_WITHOUT_JOURNAL{
    "--no-log-file", "--no-log-stderr"};

/** @brief The logging options a game shares with a supervisor at the level
 * debug. */
constexpr std::array<std::string_view, 2> DEBUG_LEVEL_ARGUMENTS{"--log-level",
                                                                "debug"};

/** @brief The number of game 3, which the command tests launch. */
constexpr std::uint32_t GAME_THREE = 3;

/** @brief The arguments `--mode game` a game command starts with, before
 * the options the game process reads. */
constexpr std::ptrdiff_t GAME_MODE_ARGUMENT_COUNT = 2;

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

/** @brief The database setting of a supervisor that names a file, and of one
 * that names none. */
constexpr std::string_view DATABASE_OF_THE_SUPERVISOR = "some/leaderboard.db";
constexpr std::string_view NO_DATABASE_SETTING{};

/** @brief Rules a creator chose in the tests, other than the default ones on
 * every field: the difficulty "hard", five lives and friendly fire on; and the
 * arguments a game is launched with for them. */
constexpr std::string_view CHOSEN_DIFFICULTY = "hard";
constexpr std::uint8_t CHOSEN_LIVES = 5;
constexpr std::string_view CHOSEN_LIVES_ARGUMENT = "5";
constexpr std::string_view FRIENDLY_FIRE_ON_ARGUMENT = "1";

/** @brief The difficulties of the shipped waves file, in the order a
 * WaveCatalog lists them, the one a game plays by default, and a difficulty it
 * has no table for. */
constexpr std::array<std::string_view, 3> SHIPPED_DIFFICULTIES{"easy", "normal",
                                                               "hard"};
constexpr std::string_view SHIPPED_DEFAULT_DIFFICULTY = "normal";
constexpr std::string_view UNKNOWN_DIFFICULTY = "nightmare";

/** @brief Lives one past the most a game accepts. */
constexpr std::uint8_t LIVES_PAST_THE_MOST = 10;
