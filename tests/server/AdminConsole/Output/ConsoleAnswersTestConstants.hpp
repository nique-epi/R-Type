#pragma once

#include <string_view>

/** @brief An answer of a command. */
constexpr std::string_view ANSWER = "no game runs";

/** @brief What the terminal receives for ANSWER above an empty typed line: the
 * row erased, the answer, a new row and the prompt "> ". */
constexpr std::string_view ANSWER_ABOVE_THE_PROMPT = "\r\x1b[Kno game runs\n> ";
