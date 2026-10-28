#pragma once

#include <chrono>

/** @brief What the failure tests say went wrong. */
constexpr const char* FAILURE_TEXT = "the network thread stopped";
constexpr const char* LATER_FAILURE_TEXT = "another part failed";

/** @brief How long a thread waits before it asks for the stop, so the thread
 * that waits is already waiting. */
constexpr std::chrono::milliseconds ASKING_DELAY{20};

/** @brief A wait for a stop announcement that nobody makes: long enough to
 * measure, short enough for a test. */
constexpr std::chrono::milliseconds UNANSWERED_WAIT{30};

/** @brief A wait for a stop announcement that was made. */
constexpr std::chrono::seconds ANSWERED_WAIT{5};
