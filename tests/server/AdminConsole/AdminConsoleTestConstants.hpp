#pragma once

#include <chrono>
#include <string_view>

/** @brief Command lines several tests of the console type. */
constexpr std::string_view PLAYERS_LINE = "players";
constexpr std::string_view NETWORK_LINE = "network";
constexpr std::string_view CLOSE_FIRST_PLACE_LINE = "close-game 1";

/** @brief What `games` and `players` write when no game runs, and `network`
 * before the games were measured. */
constexpr std::string_view NO_GAME_LINE = "no game runs";
constexpr std::string_view NO_MEASURE_LINE = "games: no measure yet";

/** @brief How often a test looks at what a thread of the console handed
 * over, until WAIT_FOR_THREADS passed. */
constexpr std::chrono::milliseconds CONSOLE_LOOK_INTERVAL{1};

/** @brief What a console at a terminal shows as it starts: its ready line, a
 * new row, and the prompt "> ". */
constexpr std::string_view READY_LINE_AND_PROMPT =
    "admin console ready: type help to list the commands\n> ";

/** @brief "games" ended by the Enter of a terminal, and its answer written
 * above the prompt: the row erased (VT100 "\r\x1b[K"), the answer, a new row
 * and the prompt. */
constexpr std::string_view GAMES_ENTERED = "games\r";
constexpr std::string_view NO_GAME_ABOVE_THE_PROMPT =
    "\r\x1b[Kno game runs\n> ";

/** @brief "gam" typed at a terminal, never ended. */
constexpr std::string_view GAM_TYPED = "gam";

/** @brief A line a game process wrote, and how the terminal shows it while
 * "gam" is typed. */
constexpr std::string_view GAME_PROCESS_LINE =
    "[INFO ] [Game] - game 1 started";
constexpr std::string_view GAME_LINE_ABOVE_GAM =
    "\r\x1b[K[INFO ] [Game] - game 1 started\n> gam";
