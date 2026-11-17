#pragma once

#include <cstdint>
#include <string_view>

/** @brief The parts of the line the tests build. */
constexpr std::string_view LINE_TIME = "2026-11-15T14:02:03.123Z";
constexpr std::string_view LINE_LEVEL = "info";
constexpr std::string_view LINE_LOGGER = "Lobby";
constexpr std::string_view LINE_MESSAGE = "player 2 joined";

/** @brief The JSON line of the parts above, members in their order. */
constexpr std::string_view PLAIN_LINE =
    R"({"time":"2026-11-15T14:02:03.123Z","level":"info","logger":"Lobby",)"
    R"("message":"player 2 joined"})";

/** @brief A message with a quote, a backslash and an end of line, which JSON
 * must escape. */
constexpr std::string_view ESCAPED_MESSAGE = "said \"hi\\\" then\nleft";

/** @brief A nickname written to end the message and add a member of its own. */
constexpr std::string_view FORGED_MESSAGE = R"(ace","event":"forged)";

/** @brief A message holding a byte that is not UTF-8, and how it reads once
 * that byte is replaced by U+FFFD. */
constexpr std::string_view INVALID_UTF8_MESSAGE = "ace\xff";
constexpr std::string_view REPLACED_UTF8_MESSAGE = "ace\xef\xbf\xbd";

/** @brief The event the tests write, the values of its fields, and the JSON
 * object eventFieldsText() makes of them. */
constexpr std::string_view EVENT_NAME = "player.left";
constexpr std::int64_t EVENT_PLAYER = 3;
constexpr std::string_view EVENT_ADDRESS = "10.0.0.7";
constexpr std::string_view EVENT_FIELDS_TEXT =
    R"({"event":"player.left","player":3,"address":"10.0.0.7","away":false})";

/** @brief The JSON line of the parts above with the fields of the event:
 * the four members of every line, then the members of the event. */
constexpr std::string_view EVENT_LINE =
    R"({"time":"2026-11-15T14:02:03.123Z","level":"info","logger":"Lobby",)"
    R"("message":"player 2 joined","event":"player.left","player":3,)"
    R"("address":"10.0.0.7","away":false})";

/** @brief A value given to a field named like a member every line has. */
constexpr std::string_view CLASHING_VALUE = "forged";
