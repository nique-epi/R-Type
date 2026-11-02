#pragma once

#include <string_view>

/**
 * @brief A setting name and a plain value.
 */
constexpr std::string_view LINE_SETTING_NAME = "port";
constexpr std::string_view LINE_PLAIN_VALUE = "6000";

/**
 * @brief The line a plain value gives.
 */
constexpr std::string_view LINE_FOR_PLAIN_VALUE = "set port 6000";

/**
 * @brief A value holding a space, and the line it gives.
 */
constexpr std::string_view LINE_SPACED_VALUE = "Captain Stardust";
constexpr std::string_view LINE_FOR_SPACED_VALUE =
    "set port \"Captain Stardust\"";

/**
 * @brief A value holding a comment mark, and the line it gives.
 */
constexpr std::string_view LINE_MARKED_VALUE = "a#b";
constexpr std::string_view LINE_FOR_MARKED_VALUE = "set port \"a#b\"";

/**
 * @brief The line an empty value gives.
 */
constexpr std::string_view LINE_FOR_EMPTY_VALUE = "set port \"\"";

/**
 * @brief A value holding a quote, and the line it gives.
 */
constexpr std::string_view LINE_QUOTED_VALUE = "a\"b";
constexpr std::string_view LINE_FOR_QUOTED_VALUE = "set port \"a\\\"b\"";

/**
 * @brief A value holding a backslash and a space, and the line it gives.
 */
constexpr std::string_view LINE_BACKSLASH_VALUE = "a\\b c";
constexpr std::string_view LINE_FOR_BACKSLASH_VALUE = "set port \"a\\\\b c\"";

/**
 * @brief A value holding a backslash and nothing that needs quotes, and the
 * line it gives.
 */
constexpr std::string_view LINE_PLAIN_BACKSLASH_VALUE = "a\\b";
constexpr std::string_view LINE_FOR_PLAIN_BACKSLASH_VALUE = "set port a\\b";

/**
 * @brief Assignments, and the line each gives.
 */
constexpr std::string_view ASSIGNMENT = "port=6000";
constexpr std::string_view ASSIGNMENT_WITH_MARK_IN_VALUE = "port=a=b";
constexpr std::string_view LINE_FOR_MARK_IN_VALUE = "set port a=b";
constexpr std::string_view ASSIGNMENT_WITH_EMPTY_VALUE = "port=";
constexpr std::string_view ASSIGNMENT_WITHOUT_MARK = "port";
constexpr std::string_view ASSIGNMENT_WITHOUT_NAME = "=6000";
