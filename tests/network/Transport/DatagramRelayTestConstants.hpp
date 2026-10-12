#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "NetworkConstants.hpp"

/**
 * @brief Room of the queues the relay tests use.
 */
constexpr std::size_t RELAY_QUEUE_CAPACITY = 4;

/**
 * @brief Address of the peer the relay tests exchange datagrams with.
 */
constexpr std::string_view PEER_ADDRESS = "192.0.2.7";

/**
 * @brief Port of the peer the relay tests exchange datagrams with.
 */
constexpr std::uint16_t PEER_PORT = 4243;

/**
 * @brief Port of a second peer, so two destinations tell apart.
 */
constexpr std::uint16_t OTHER_PEER_PORT = 4244;

/**
 * @brief A byte the receive buffer holds before a test overwrites it.
 */
constexpr std::byte RECEIVED_BYTE{0x2A};

/**
 * @brief The byte a test writes over the receive buffer after the handler
 * returned.
 */
constexpr std::byte OVERWRITTEN_BYTE{0x00};

/**
 * @brief Number of payload bytes the relay tests send and receive.
 */
constexpr std::size_t RELAY_PAYLOAD_SIZE = 3;

/**
 * @brief One byte more than a socket sends in one datagram: the smallest
 * payload a socket refuses.
 */
constexpr std::size_t REFUSED_PAYLOAD_SIZE =
    rtype::network::MAX_DATAGRAM_SIZE + 1;
