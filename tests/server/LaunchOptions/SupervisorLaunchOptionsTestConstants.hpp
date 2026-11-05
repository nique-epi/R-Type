#pragma once

#include <cstdint>
#include <string_view>

/** @brief The values of a supervisor link in the tests: the port and the token
 * as the command line writes them, and as the options read them. */
constexpr std::string_view SUPERVISOR_PORT_TEXT = "4242";
constexpr std::uint16_t SUPERVISOR_PORT_VALUE = 4242;
constexpr std::string_view LINK_TOKEN_TEXT = "987654321";
constexpr std::uint64_t LINK_TOKEN_VALUE = 987654321;

/** @brief A link token that is not one, and a supervisor port no one can reach.
 */
constexpr std::string_view ZERO_TEXT = "0";
