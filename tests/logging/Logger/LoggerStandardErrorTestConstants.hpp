#pragma once

#include <string_view>

/** @brief The name of the logger of the tests, and how the end of its line
 * reads after the text DISK_FULL_TEXT. */
constexpr std::string_view STANDARD_ERROR_LOGGER_NAME = "Test";
constexpr std::string_view DISK_FULL_TEXT = "disk full";
constexpr std::string_view DISK_FULL_LINE_END = "[Test] - disk full";

/** @brief A line a game process wrote, which reaches standard error as it
 * is. */
constexpr std::string_view GAME_LINE = "[INFO ] [Game] - game 3 started";

/** @brief How a terminal is told to write in red, the colour of an error (ANSI
 * SGR 31), and back to its default colour (SGR 0). */
constexpr std::string_view ERROR_COLOR_START = "\x1b[31m";
constexpr std::string_view DEFAULT_COLOR = "\x1b[0m";
