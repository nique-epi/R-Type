#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::server {

/** @brief The name of the journal lines of the console. */
constexpr std::string_view ADMIN_CONSOLE_LOGGER_NAME = "Console";

/** @brief The lines the console thread may leave for the simulation thread
 * between two ticks; past it the oldest is dropped. A person types far fewer;
 * only an input redirected from a file reaches it. */
constexpr std::size_t CONSOLE_LINE_CAPACITY = 64;

/** @brief The bytes one read of the input may take. */
constexpr std::size_t CONSOLE_READ_SIZE = 512;

/** @brief The longest line kept, its '\r' included; a longer one is dropped
 * whole, never cut. */
constexpr std::size_t MAXIMUM_CONSOLE_LINE_LENGTH = 1024;

/** @brief The end of a line, and the character a Windows terminal puts
 * before it. */
constexpr char LINE_END = '\n';
constexpr char CARRIAGE_RETURN = '\r';

/** @brief How long the console waits before it reads again when the system
 * refuses the read because the supervisor runs in the background of a
 * shell. */
constexpr std::chrono::milliseconds CONSOLE_BACKGROUND_WAIT{500};

/** @brief How long, on Windows, the stop waits before it cancels again a read
 * that had not started yet. */
constexpr std::chrono::milliseconds CONSOLE_CANCEL_RETRY{1};

/** @brief The byte the stop writes on the pipe that wakes select(), on
 * Linux and macOS. */
constexpr char WAKE_BYTE = 0;

/** @brief What the commands write when there is nothing to list. */
constexpr std::string_view NO_GAME_TEXT = "no game runs";
constexpr std::string_view NO_PILOT_TEXT = "no pilot";
constexpr std::string_view NO_MEASURE_TEXT = "games: no measure yet";

/** @brief The decimals the console writes a measure with. */
constexpr int MEASURE_DECIMALS = 1;

/** @brief The names of the commands of the console. */
constexpr std::string_view GAMES_COMMAND_NAME = "games";
constexpr std::string_view PLAYERS_COMMAND_NAME = "players";
constexpr std::string_view CLOSE_GAME_COMMAND_NAME = "close-game";

/** @brief The argument of close-game, as its help and its refusal show it. */
constexpr std::string_view CLOSE_GAME_ARGUMENTS = "<place>";
constexpr std::string_view NETWORK_COMMAND_NAME = "network";
constexpr std::string_view HELP_COMMAND_NAME = "help";
constexpr std::string_view KICK_COMMAND_NAME = "kick";

/** @brief The argument of kick, as its help and its refusal show it. */
constexpr std::string_view KICK_ARGUMENTS = "<nickname>";

}  // namespace rtype::server
