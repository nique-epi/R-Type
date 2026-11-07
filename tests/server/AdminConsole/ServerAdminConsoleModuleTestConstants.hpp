#pragma once

#include <string_view>

/** @brief A mistyped command, and the line the interpreter answers it
 * with. */
constexpr std::string_view MISTYPED_GAMES_LINE = "gmaes";
constexpr std::string_view MISTYPED_GAMES_ANSWER =
    "unknown command 'gmaes' in line 'gmaes'";
