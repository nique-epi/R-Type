#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>

namespace rtype::network {

/**
 * @brief Largest UDP payload a datagram may carry, in bytes.
 *
 * Every IPv6 link carries packets of 1280 bytes; minus the IPv6 and UDP
 * headers that leaves 1232 bytes, and 1200 keeps room for extension headers.
 * See the Transport section of the protocol page.
 */
constexpr std::size_t MAX_DATAGRAM_SIZE = 1200;

/**
 * @brief Size of the buffer a socket receives into: one byte more than the
 *        largest datagram.
 *
 * A datagram too large for the buffer is cut down to the size of the buffer,
 * without an error, on Linux and macOS as on Windows. The extra byte is what
 * tells such a datagram apart from one of exactly MAX_DATAGRAM_SIZE bytes.
 */
constexpr std::size_t RECEIVE_BUFFER_SIZE = MAX_DATAGRAM_SIZE + 1;

/**
 * @brief Port the server listens on when no port is given at launch.
 */
constexpr std::uint16_t DEFAULT_SERVER_PORT = 4242;

/**
 * @brief Lowest port a program may be given at launch. Port 0 asks the system
 * for any free port, which the players could not know.
 */
constexpr std::uint16_t LOWEST_PORT = 1;

/**
 * @brief Highest port a UDP datagram can name.
 */
constexpr std::uint16_t HIGHEST_PORT =
    std::numeric_limits<std::uint16_t>::max();

/**
 * @brief Port that lets the system pick a free port when a socket is opened.
 */
constexpr std::uint16_t ANY_PORT = 0;

/**
 * @brief Address that binds a socket to every IPv4 interface of the machine.
 */
constexpr std::string_view ANY_IPV4_ADDRESS = "0.0.0.0";

/**
 * @brief Address of the machine itself, over IPv4.
 */
constexpr std::string_view LOOPBACK_IPV4_ADDRESS = "127.0.0.1";

/**
 * @brief Number of decimal numbers in an IPv4 address written as text.
 */
constexpr std::size_t IPV4_ADDRESS_PART_COUNT = 4;

/**
 * @brief Largest value of one of the four numbers of an IPv4 address.
 */
constexpr unsigned int IPV4_ADDRESS_PART_MAXIMUM =
    std::numeric_limits<std::uint8_t>::max();

/**
 * @brief Character between the numbers of an IPv4 address written as text.
 */
constexpr char IPV4_ADDRESS_SEPARATOR = '.';

/**
 * @brief Module name the network classes write their log lines under.
 */
constexpr std::string_view NETWORK_LOGGER_NAME = "Network";

/**
 * @brief Shortest time between two log lines about the received bytes one
 * part of a program rejects, such as a datagram too large or a message no
 * handler reads: anyone can send them, so a flood of them writes one line per
 * interval, which says how many were rejected since the previous one.
 */
constexpr std::chrono::seconds REJECTION_LINE_INTERVAL{10};

/**
 * @brief Why a receiver drops a datagram whose header is not of this protocol
 * and version, or has a flag set, as its rejection lines say.
 */
constexpr std::string_view FOREIGN_HEADER_DROP_REASON =
    "its header is not of this protocol and version, or has a flag set";

}  // namespace rtype::network
