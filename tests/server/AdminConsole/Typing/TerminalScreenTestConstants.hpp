#pragma once

#include <string_view>

/** @brief What erases the row of the prompt (VT100 "\r\x1b[K"). */
constexpr std::string_view ERASED_ROW = "\r\x1b[K";

/** @brief Two lines a game wrote, relayed as one text, and the rows they must
 * take: each from the first column, then the prompt. */
constexpr std::string_view TWO_GAME_LINES = "game 1 started\ngame 2 started";
constexpr std::string_view TWO_GAME_ROWS =
    "game 1 started\r\ngame 2 started\r\n> ";
