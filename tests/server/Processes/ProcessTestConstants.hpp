#pragma once

#include <chrono>

/** @brief How long a test waits for a process to start, end or die. */
constexpr std::chrono::seconds PROCESS_WAIT{10};

/** @brief How often a test asks a process whether it ended. */
constexpr std::chrono::milliseconds PROCESS_POLL_INTERVAL{10};

/** @brief The exit code the test child ends with when asked to. */
constexpr int EXIT_CODE_OF_THE_CHILD = 7;
