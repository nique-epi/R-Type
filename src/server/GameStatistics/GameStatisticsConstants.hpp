#pragma once

#include <chrono>
#include <string_view>

namespace rtype::server {

/**
 * @brief The name of the plugin, of its file (game-statistics.so, or .dll on
 * Windows) and of its module: `plugin game-statistics` loads it, and
 * `module game-statistics off` turns it off.
 */
constexpr std::string_view GAME_STATISTICS_MODULE_NAME = "game-statistics";

/** @brief The name of the logger the plugin writes its lines with. */
constexpr std::string_view GAME_STATISTICS_LOGGER_NAME = "GameStatistics";

/**
 * @brief The name of the file while GAME_STATISTICS_FILE_SETTING_NAME (of
 * ServerConstants.hpp) is empty, next to the executable, around the port of the
 * server: r-type_statistics_4242.csv. Two servers on one machine listen on two
 * ports, so they never write the same file.
 */
constexpr std::string_view GAME_STATISTICS_FILE_PREFIX = "r-type_statistics_";
constexpr std::string_view GAME_STATISTICS_FILE_SUFFIX = ".csv";

/**
 * @brief What goes between the name of the file
 * GAME_STATISTICS_FILE_SETTING_NAME gives and the port of the game, before the
 * extension: stats.csv gives stats_4242.csv to the game on port 4242.
 */
constexpr std::string_view GAME_STATISTICS_PORT_SEPARATOR = "_";

/**
 * @brief The first line of the file, and the first column of each row: a
 * Bydo killed in a game, by a player, worth its points; or the total of the
 * points of a game that ended, with no player.
 */
constexpr std::string_view GAME_STATISTICS_CSV_HEADER =
    "event,game,player,points";
constexpr std::string_view KILL_EVENT = "kill";
constexpr std::string_view TOTAL_EVENT = "total";

/**
 * @brief The game time the rows wait in memory between two writes. The module
 * adds the time of each pass of the outputs stage, writes once the sum reaches
 * it, then starts over from zero. At 60 ticks per second, 60 ticks last
 * 999.99996 ms, so the rows go out every 61 ticks (1016.7 ms) while the server
 * keeps its pace, never twice within a second.
 */
constexpr std::chrono::seconds GAME_STATISTICS_WRITE_PERIOD{1};

}  // namespace rtype::server
