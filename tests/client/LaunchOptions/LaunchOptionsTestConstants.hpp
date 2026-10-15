#pragma once

#include <cstdint>
#include <string_view>

/**
 * @brief An IPv4 address given after `-h`.
 */
constexpr std::string_view ADDRESS_ARGUMENT = "192.168.1.20";

/**
 * @brief A second IPv4 address given after `-h`.
 */
constexpr std::string_view OTHER_ADDRESS_ARGUMENT = "192.168.1.21";

/**
 * @brief A host name given after `-h`, which the client does not resolve.
 */
constexpr std::string_view HOST_NAME_ARGUMENT = "localhost";

/**
 * @brief A port given after `-p`, written in digits.
 */
constexpr std::string_view PORT_ARGUMENT = "4243";

/**
 * @brief The port PORT_ARGUMENT names.
 */
constexpr std::uint16_t PORT_OF_PORT_ARGUMENT = 4243;

/**
 * @brief A second port given after `-p`.
 */
constexpr std::string_view OTHER_PORT_ARGUMENT = "4244";

/**
 * @brief The port OTHER_PORT_ARGUMENT names.
 */
constexpr std::uint16_t PORT_OF_OTHER_PORT_ARGUMENT = 4244;

/**
 * @brief A value after `-p` that is no port.
 */
constexpr std::string_view NOT_A_PORT_ARGUMENT = "abc";

/**
 * @brief An option no program of the repository defines.
 */
constexpr std::string_view UNKNOWN_OPTION = "-x";

/**
 * @brief The line of the usage that gives the default address.
 */
constexpr std::string_view DEFAULT_ADDRESS_LINE =
    "IPv4 address of the server, such as 127.0.0.1. Host names are not "
    "resolved. Default: 127.0.0.1.";

/**
 * @brief The line of the usage that gives the default port.
 */
constexpr std::string_view DEFAULT_PORT_LINE =
    "UDP port of the server, from 1 to 65535. Default: 4242.";
