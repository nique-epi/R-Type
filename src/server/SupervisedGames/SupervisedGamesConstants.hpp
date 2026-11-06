#pragma once

#include <chrono>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief How long a game process may take to send its first status before the
 * supervisor kills it. */
constexpr engine::Duration GAME_START_TIMEOUT = std::chrono::seconds(10);

/**
 * @brief How long a seat the supervisor gave a player stays held while the
 * game does not report that player: the handshake of a redirected player with
 * the game takes two round trips, a lost request costs it
 * network::CONNECTION_REQUEST_INTERVAL (500 ms), and the game reports a new
 * player at once. Three seconds leave room for four lost requests; a player
 * still absent then is not coming, and its seat is free again.
 */
constexpr engine::Duration SEAT_RESERVATION_TIMEOUT = std::chrono::seconds(3);

/** @brief How long the supervisor waits, after telling the games to stop, for
 * them to end before it kills the ones that remain. */
constexpr engine::Duration STOP_GRACE_WAIT = std::chrono::seconds(3);

/** @brief How often the supervisor looks, while it waits, whether the games
 * ended. */
constexpr engine::Duration STOP_POLL_INTERVAL = std::chrono::milliseconds(50);

/** @brief The name the supervisor writes its journal lines under. */
constexpr std::string_view SUPERVISOR_LOGGER_NAME = "Supervisor";

/** @brief The journal file of game n is GAME_LOG_FILE_PREFIX, n and
 * GAME_LOG_FILE_SUFFIX. */
constexpr std::string_view GAME_LOG_FILE_PREFIX = "r-type_game_";
constexpr std::string_view GAME_LOG_FILE_SUFFIX = ".log";

/** @brief The file of commands the supervisor on port p writes for its games
 * is GAME_CONFIGURATION_FILE_PREFIX, p and GAME_CONFIGURATION_FILE_SUFFIX,
 * next to the executable: two supervisors on one machine listen on two ports,
 * so they never write the same file. */
constexpr std::string_view GAME_CONFIGURATION_FILE_PREFIX = "r-type_games_";
constexpr std::string_view GAME_CONFIGURATION_FILE_SUFFIX = ".cfg";

/** @brief How the journal names the lifecycle of a game. */
constexpr std::string_view STARTING_TEXT = "starting";
constexpr std::string_view WAITING_FOR_PLAYERS_TEXT = "waiting for players";
constexpr std::string_view IN_PROGRESS_TEXT = "in progress";
constexpr std::string_view FINISHED_TEXT = "finished";

}  // namespace rtype::server
