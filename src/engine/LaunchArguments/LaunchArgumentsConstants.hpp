#pragma once

#include <string_view>

namespace rtype::engine {

/**
 * @brief Launch option followed by a UDP port, the one to listen on or the
 * one of the server to join.
 */
constexpr std::string_view PORT_OPTION = "-p";

/**
 * @brief Launch option that prints the usage and starts nothing.
 */
constexpr std::string_view HELP_OPTION = "--help";

/**
 * @brief What starts the line of an option in a usage.
 */
constexpr std::string_view USAGE_OPTION_INDENT = "  ";

/**
 * @brief What starts the line that describes an option, under the option.
 */
constexpr std::string_view USAGE_DESCRIPTION_INDENT = "      ";

}  // namespace rtype::engine
