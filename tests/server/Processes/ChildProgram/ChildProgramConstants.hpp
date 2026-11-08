#pragma once

#include <chrono>
#include <string_view>

/** @brief The commands the test child understands: end at once with the code
 * that follows, write a line on each of its outputs then end, or sleep. */
constexpr std::string_view EXIT_COMMAND = "exit";
constexpr std::string_view SLEEP_COMMAND = "sleep";
constexpr std::string_view PRINT_COMMAND = "print";

/** @brief The number of arguments of the exit command, the program included. */
constexpr int ARGUMENTS_OF_EXIT = 3;

/** @brief The number of arguments of the print command, the program included.
 */
constexpr int ARGUMENTS_OF_PRINT = 2;

/** @brief What the print command writes on the standard output, then on the
 * standard error. */
constexpr std::string_view CHILD_OUTPUT_LINE = "child out";
constexpr std::string_view CHILD_ERROR_LINE = "child err";

/** @brief How long the test child sleeps: far longer than any test waits. */
constexpr std::chrono::seconds CHILD_SLEEP_DURATION{60};
