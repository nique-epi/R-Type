#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
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

/** @brief What the console shows where the administrator types. */
constexpr std::string_view CONSOLE_PROMPT = "> ";

/** @brief The line the console writes once it reads a terminal. */
constexpr std::string_view CONSOLE_READY_TEXT =
    "admin console ready: type help to list the commands";

/** @brief Takes the cursor back to the start of its row and erases the row,
 * before a line is written above the prompt (VT100 "erase in line"). */
constexpr std::string_view ERASE_ROW = "\r\x1b[K";

/** @brief Erases the character left of the cursor, as a terminal echoes a
 * backspace. */
constexpr std::string_view ERASE_CHARACTER = "\b \b";

/** @brief Ends a row of the terminal. */
constexpr std::string_view ROW_END = "\n";

/** @brief The keys the line editor reads apart from the text. */
constexpr char ESCAPE = '\x1b';
constexpr char BACKSPACE = '\b';
constexpr char DELETE_CHARACTER = '\x7f';
constexpr char FIRST_PRINTABLE = ' ';

/** @brief The bytes after ESCAPE that open the sequence of an arrow or a
 * function key, and the range of the byte that ends it (ECMA-48). */
constexpr char CONTROL_SEQUENCE_OPENER = '[';
constexpr char FUNCTION_SEQUENCE_OPENER = 'O';
constexpr char FIRST_FINAL_BYTE = '@';
constexpr char LAST_FINAL_BYTE = '~';

/** @brief The two top bits of a byte that continues a UTF-8 character. */
constexpr unsigned char UTF8_TOP_BITS = 0xC0;
constexpr unsigned char UTF8_CONTINUATION = 0x80;

/** @brief How a read of a terminal in typing mode returns: as soon as one byte
 * came (VMIN), with no timer (VTIME). */
constexpr unsigned char TYPING_MINIMUM_BYTES = 1;
constexpr unsigned char TYPING_TIMEOUT = 0;

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

/** @brief The names of the commands of the bans, and their arguments as their
 * help and their refusals show them. */
constexpr std::string_view BAN_COMMAND_NAME = "ban";
constexpr std::string_view BAN_ARGUMENTS = "<nickname|address> [<length>]";
constexpr std::string_view UNBAN_COMMAND_NAME = "unban";
constexpr std::string_view UNBAN_ARGUMENTS = "<nickname|address>";
constexpr std::string_view BANS_COMMAND_NAME = "bans";

/** @brief What bans writes when no ban is in force. */
constexpr std::string_view NO_BAN_TEXT = "no ban";

/** @brief How the console names the target of a ban. */
constexpr std::string_view NICKNAME_TARGET_WORD = "nickname";
constexpr std::string_view ADDRESS_TARGET_WORD = "address";

/** @brief The letters a length of ban ends with, and what each stands for: 30m,
 * 2h, 7d. */
constexpr char MINUTE_LETTER = 'm';
constexpr char HOUR_LETTER = 'h';
constexpr char DAY_LETTER = 'd';
constexpr std::chrono::seconds BAN_MINUTE = std::chrono::minutes{1};
constexpr std::chrono::seconds BAN_HOUR = std::chrono::hours{1};
constexpr std::chrono::seconds BAN_DAY = std::chrono::days{1};

/** @brief The largest count of a length, before its letter: 9999 days are
 * about 27 years, far from the limits of std::chrono::sys_seconds. */
constexpr std::uint32_t MAXIMUM_BAN_COUNT = 9999;

/** @brief How the console writes the days, hours and minutes of a length. */
constexpr std::string_view DAYS_UNIT_TEXT = " d";
constexpr std::string_view HOURS_UNIT_TEXT = " h";
constexpr std::string_view MINUTES_UNIT_TEXT = " min";

}  // namespace rtype::server
