#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "NetworkConstants.hpp"

/**
 * @brief Address of the machine itself: a test never opens a port to the
 *        network.
 */
constexpr std::string_view LOOPBACK_ADDRESS = "127.0.0.1";

/**
 * @brief A name, not an address: sockets never resolve one.
 */
constexpr std::string_view HOST_NAME = "localhost";

/**
 * @brief Port that lets the system pick a free port.
 */
constexpr std::uint16_t ANY_PORT = 0;

/**
 * @brief How long a test waits for a datagram; only a failing test waits that
 *        long.
 */
constexpr std::chrono::seconds DATAGRAM_TIMEOUT{5};

/**
 * @brief Time given to the system to deliver a datagram sent on the loopback
 *        address, when a test needs it delivered before the next step.
 */
constexpr std::chrono::milliseconds LOOPBACK_DELIVERY_TIME{100};

/**
 * @brief Size of the payload most tests exchange.
 */
constexpr std::size_t SMALL_PAYLOAD_SIZE = 16;

/**
 * @brief A datagram size well beyond the receive buffer of a socket.
 */
constexpr std::size_t OVERSIZED_PAYLOAD_SIZE =
    2 * rtype::network::MAX_DATAGRAM_SIZE;
