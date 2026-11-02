#pragma once

#include <string_view>

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
 * @brief The command that gives a declared setting a value.
 */
constexpr std::string_view SET_COMMAND_NAME = "set";

/**
 * @brief The command that turns a module of the program on or off.
 */
constexpr std::string_view MODULE_COMMAND_NAME = "module";

/**
 * @brief The word that turns a module on.
 */
constexpr std::string_view SWITCH_ON_WORD = "on";

/**
 * @brief The word that turns a module off.
 */
constexpr std::string_view SWITCH_OFF_WORD = "off";

/**
 * @brief The value that clears a setting allowed to hold nothing.
 */
constexpr std::string_view NO_VALUE_WORD = "none";

}  // namespace rtype::engine
