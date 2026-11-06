#pragma once

#include <chrono>
#include <string_view>

/** @brief The commands the test child understands: end at once with the code
 * that follows, or sleep. */
constexpr std::string_view EXIT_COMMAND = "exit";
constexpr std::string_view SLEEP_COMMAND = "sleep";

/** @brief The number of arguments of the exit command, the program included. */
constexpr int ARGUMENTS_OF_EXIT = 3;

/** @brief How long the test child sleeps: far longer than any test waits. */
constexpr std::chrono::seconds CHILD_SLEEP_DURATION{60};
