#pragma once

#include <cstdint>
#include <string_view>

/** @brief The logger of the tests, the event it writes, and the value of its
 * one field. */
constexpr std::string_view EVENT_LOGGER_NAME = "Sessions";
constexpr std::string_view TIMED_OUT_EVENT = "player.timed_out";
constexpr std::int64_t TIMED_OUT_PLAYER = 4;

/** @brief A message that starts like the size the queue writes before it, and
 * holds what looks like the fields of an event. */
constexpr std::string_view TRICKY_MESSAGE = R"(12:34 {"event":"x"} gone)";

/** @brief How the line of the event ends on standard error: the message alone,
 * as a warning of EVENT_LOGGER_NAME would read. */
constexpr std::string_view TRICKY_TEXT_LINE_END =
    R"([WARN ] [Sessions] - 12:34 {"event":"x"} gone)";

/** @brief The value the tests give after `--log-format`. */
constexpr std::string_view JSON_FORMAT_VALUE = "json";
