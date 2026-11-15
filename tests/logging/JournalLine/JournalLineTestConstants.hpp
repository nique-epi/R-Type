#pragma once

#include <string_view>

/** @brief The parts of the line the tests build. */
constexpr std::string_view LINE_TIME = "2026-11-15T14:02:03.123Z";
constexpr std::string_view LINE_LEVEL = "info";
constexpr std::string_view LINE_LOGGER = "Lobby";
constexpr std::string_view LINE_MESSAGE = "player 2 joined";

/** @brief A message with a quote, a backslash and an end of line, which JSON
 * must escape. */
constexpr std::string_view ESCAPED_MESSAGE = "said \"hi\\\" then\nleft";

/** @brief A nickname written to end the message and add a member of its own. */
constexpr std::string_view FORGED_MESSAGE = R"(ace","event":"forged)";

/** @brief A message holding a byte that is not UTF-8, and how it reads once
 * that byte is replaced by U+FFFD. */
constexpr std::string_view INVALID_UTF8_MESSAGE = "ace\xff";
constexpr std::string_view REPLACED_UTF8_MESSAGE = "ace\xef\xbf\xbd";
