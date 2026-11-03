#pragma once

namespace rtype::engine {

/**
 * @brief The character that starts a comment in a line of commands; what
 * follows it, outside quotes, is ignored.
 */
constexpr char COMMENT_START = '#';

/**
 * @brief The character that wraps a word holding spaces in a line of commands.
 */
constexpr char QUOTE = '"';

/**
 * @brief The character that, inside quotes, makes the quote or the backslash
 * after it part of the word.
 */
constexpr char ESCAPE = '\\';

}  // namespace rtype::engine
