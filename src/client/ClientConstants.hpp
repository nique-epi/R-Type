#pragma once

#include <string_view>
#include "NetworkConstants.hpp"

namespace rtype::client {

/**
 * @brief Name of the client program, at the start of its usage and of the
 * errors it prints about its arguments.
 */
constexpr std::string_view PROGRAM_NAME = "r-type_client";

/**
 * @brief Launch option followed by the IPv4 address of the server to join.
 */
constexpr std::string_view ADDRESS_OPTION = "-h";

/**
 * @brief Address of the server to join when none is given at launch: a
 * server on the same machine.
 */
constexpr std::string_view DEFAULT_SERVER_ADDRESS =
    network::LOOPBACK_IPV4_ADDRESS;

/**
 * @brief Module name the client writes the log lines of its launch under.
 */
constexpr std::string_view CLIENT_LOGGER_NAME = "Client";

}  // namespace rtype::client
