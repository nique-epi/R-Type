#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "NetworkConstants.hpp"

/**
 * @brief A name, not an address: sockets never resolve one.
 */
constexpr std::string_view HOST_NAME = "localhost";

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

/**
 * @brief How many datagrams too large a test sends at once, to check that
 * they share one log line.
 */
constexpr std::size_t OVERSIZED_DATAGRAM_COUNT = 3;

/**
 * @brief Words of every log line about a datagram too large, which tell those
 * lines apart from the others.
 */
constexpr std::string_view OVERSIZED_DATAGRAM_LINE_TEXT =
    "dropped a datagram larger than";

/**
 * @brief Words of every log line about a message a dispatcher read without a
 * handler for its type.
 */
constexpr std::string_view SKIPPED_MESSAGE_LINE_TEXT = "skipped a message";
