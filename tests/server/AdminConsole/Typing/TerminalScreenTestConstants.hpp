#pragma once

#include <string_view>

/** @brief What the console writes as it opens: its ready line, a new row, and
 * the prompt "> ". */
constexpr std::string_view OPENING =
    "admin console ready: type help to list the commands\n> ";

/** @brief A line of the journal, and what the terminal receives for it while
 * "gam" is typed: the row erased (VT100 "\r\x1b[K"), the line, a new row, the
 * prompt and "gam" again. */
constexpr std::string_view JOURNAL_LINE = "[INFO ] [Game] - game 1 started";
constexpr std::string_view JOURNAL_LINE_ABOVE_GAM =
    "\r\x1b[K[INFO ] [Game] - game 1 started\n> gam";

/** @brief Keys the tests type, and the line they end. */
constexpr std::string_view SCREEN_HALF_OF_GAMES = "gam";
constexpr std::string_view SCREEN_GAMES_ENDED = "games\r";
constexpr std::string_view SCREEN_GAMES = "games";

/** @brief What erases the row of the prompt. */
constexpr std::string_view ERASED_ROW = "\r\x1b[K";
