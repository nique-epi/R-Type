#pragma once

#include <cstdint>

/** @brief The exit code of a recorded child that was terminated: the number of
 * SIGKILL. */
constexpr int KILLED_EXIT_CODE = 9;

/** @brief The process id of the first recorded child. */
constexpr std::int64_t FIRST_PROCESS_ID = 1000;
