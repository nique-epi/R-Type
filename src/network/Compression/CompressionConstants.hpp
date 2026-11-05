#pragma once

#include <cstddef>
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief The most bytes the body of a datagram holds: the datagram less its
 * packet header. A decoder never produces more.
 */
constexpr std::size_t MAXIMUM_BODY_SIZE =
    MAX_DATAGRAM_SIZE - PACKET_HEADER_SIZE;

}  // namespace rtype::network
