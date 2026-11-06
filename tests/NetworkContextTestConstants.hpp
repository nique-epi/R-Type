#pragma once

#include <chrono>

/**
 * @brief How long a test leaves a context with no work before it posts again:
 * far longer than run() takes to return once its last task ran.
 */
constexpr std::chrono::milliseconds IDLE_LOOP_PAUSE{50};
