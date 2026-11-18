#pragma once

#include <chrono>
#include <cstdint>

/** @brief How often the job of the tests looks for rounds, how often the test
 * reads what it drew, and how long it waits at most. */
constexpr std::chrono::milliseconds QUICK_PASS{1};
constexpr std::chrono::milliseconds DRAWN_POLL{5};
constexpr int DRAWN_POLLS = 400;

/** @brief The second room of the casino of the tests. */
constexpr std::uint8_t SECOND_ROOM = 2;
