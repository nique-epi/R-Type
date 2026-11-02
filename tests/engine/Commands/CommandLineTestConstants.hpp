#pragma once

#include <string_view>

/**
 * @brief A line of three words.
 */
constexpr std::string_view THREE_WORDS_LINE = "set  port\t6000";

/**
 * @brief A line whose second word is wrapped in quotes and holds a space.
 */
constexpr std::string_view QUOTED_LINE = "set nickname \"Captain Stardust\"";

/**
 * @brief A line with a comment after its words.
 */
constexpr std::string_view TRAILING_COMMENT_LINE = "set port 6000 # default";

/**
 * @brief A line whose comment mark is inside quotes.
 */
constexpr std::string_view QUOTED_MARK_LINE = "set nickname \"a#b\"";

/**
 * @brief A line that leaves its quote open.
 */
constexpr std::string_view OPEN_QUOTE_LINE = "set nickname \"Captain Star";

/**
 * @brief A line holding an empty quoted word.
 */
constexpr std::string_view EMPTY_QUOTED_LINE = "set nickname \"\"";
