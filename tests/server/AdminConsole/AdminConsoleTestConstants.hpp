#pragma once

#include <chrono>
#include <string_view>

/** @brief Command lines several tests of the console type. */
constexpr std::string_view GAMES_LINE = "games";
constexpr std::string_view PLAYERS_LINE = "players";
constexpr std::string_view NETWORK_LINE = "network";
constexpr std::string_view CLOSE_FIRST_PLACE_LINE = "close-game 1";

/** @brief The line that turns the console off in server.cfg. */
constexpr std::string_view CONSOLE_OFF_LINE = "module \"admin console\" off";

/** @brief What `games` and `players` write when no game runs, and `network`
 * before the games were measured. */
constexpr std::string_view NO_GAME_LINE = "no game runs";
constexpr std::string_view NO_MEASURE_LINE = "games: no measure yet";

/** @brief How often a test looks at what a thread of the console handed
 * over, until WAIT_FOR_THREADS passed. */
constexpr std::chrono::milliseconds CONSOLE_LOOK_INTERVAL{1};
