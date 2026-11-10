#pragma once

#include <array>
#include <cstddef>
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief The most bytes the body of a datagram holds: the datagram less its
 * packet header. A decoder never produces more.
 */
constexpr std::size_t MAXIMUM_BODY_SIZE =
    MAX_DATAGRAM_SIZE - PACKET_HEADER_SIZE;

/**
 * @brief The messages a supervisor and its games send each other. Both ends
 * read them through a CompressingDatagramSocket, which drops what is
 * compressed, so a datagram holding one of them always stays plain.
 */
constexpr std::array SUPERVISOR_LINK_MESSAGES{
    MessageType::GameStatus, MessageType::GameStatusAcknowledged,
    MessageType::Disconnection, MessageType::ExpulsionOrder};

}  // namespace rtype::network
