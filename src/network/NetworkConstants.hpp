#pragma once

#include <cstddef>
#include <cstdint>
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
 * @brief Address that binds a socket to every IPv4 interface of the machine.
 */
constexpr std::string_view ANY_IPV4_ADDRESS = "0.0.0.0";

/**
 * @brief Module name the network classes write their log lines under.
 */
constexpr std::string_view NETWORK_LOGGER_NAME = "Network";

}  // namespace rtype::network
