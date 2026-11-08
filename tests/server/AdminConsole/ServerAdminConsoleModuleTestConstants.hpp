#pragma once

#include <string_view>

/** @brief A mistyped command, and the line the interpreter answers it
 * with. */
constexpr std::string_view MISTYPED_GAMES_LINE = "gmaes";
constexpr std::string_view MISTYPED_GAMES_ANSWER =
    "unknown command 'gmaes' in line 'gmaes'";

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

/** @brief "gam" typed at a terminal, never ended, and how the prompt shows
 * it. */
constexpr std::string_view GAM_TYPED = "gam";
constexpr std::string_view PROMPT_AND_GAM = "> gam";

/** @brief A line a game process wrote, and how the terminal shows it while
 * "gam" is typed. */
constexpr std::string_view GAME_PROCESS_LINE =
    "[INFO ] [Game] - game 1 started";
constexpr std::string_view GAME_LINE_ABOVE_GAM =
    "\r\x1b[K[INFO ] [Game] - game 1 started\n> gam";
