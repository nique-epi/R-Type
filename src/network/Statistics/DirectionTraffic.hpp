#pragma once

#include <array>
#include "ProtocolConstants.hpp"
#include "TrafficFigures.hpp"

namespace rtype::network {

/**
 * @brief What crossed a socket in one direction. Bytes are those of the UDP
 * payload: over IPv4, the headers of IP and UDP add at least 28 bytes per
 * datagram.
 */
struct DirectionTraffic {
  /** @brief Every datagram, with all its bytes: the packet header, the prefixes
   * and the payloads of its messages. */
  TrafficFigures datagrams;
  /** @brief The datagrams among them whose messages are not counted: not of
   * this protocol and version, or whose sizes do not add up to their end. */
  TrafficFigures unreadable;
  /** @brief The messages of the other datagrams, indexed by type byte, each
   * with its prefix and its payload; the packet header is in no type. */
  std::array<TrafficFigures, MESSAGE_TYPE_COUNT> messages;
};

}  // namespace rtype::network
