#pragma once

#include <string_view>

namespace rtype::engine {

/**
 * @brief The command that gives a declared setting a value.
 */
constexpr std::string_view SET_COMMAND_NAME = "set";

/**
 * @brief The value that clears a setting allowed to hold nothing.
 */
constexpr std::string_view NO_VALUE_WORD = "none";

}  // namespace rtype::engine
