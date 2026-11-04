#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::logging::testing {

/**
 * @brief The logger name and the interval of the throttled log of the tests.
 */
constexpr std::string_view THROTTLED_LOGGER_NAME = "Throttled";
constexpr std::chrono::nanoseconds THROTTLED_INTERVAL =
    std::chrono::seconds{10};
constexpr std::chrono::nanoseconds HALF_INTERVAL = THROTTLED_INTERVAL / 2;

/**
 * @brief When the first event of a test happens, measured from the origin the
 * test picked; any time works, 0 included.
 */
constexpr std::chrono::nanoseconds FIRST_EVENT_TIME = std::chrono::seconds{3};

/**
 * @brief The text each event of the tests writes, and how many events happen
 * between two lines when a test floods the log.
 */
constexpr std::string_view EVENT_TEXT = "dropped a datagram from 10.0.0.7";
constexpr std::size_t FLOODED_EVENT_COUNT = 5;

/**
 * @brief What surrounds the count at the end of a line: " (3 since the
 * previous line)" for three events.
 */
constexpr std::string_view COUNT_ENDING_OPENING = " (";
constexpr std::string_view COUNT_ENDING_CLOSING = " since the previous line)";

}  // namespace rtype::logging::testing
