#pragma once

#include <cstdint>
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief The PACKET_HEADER_SIZE bytes every datagram starts with, in the order
 * of the Datagram section of the protocol page.
 *
 * The default values make a header of the version this code speaks, with no
 * session yet: a sender sets the session token and the sequence it knows.
 * flags, acknowledgedSequence and acknowledgementBits stay 0 in version 0.
 */
struct PacketHeader {
  std::uint32_t protocolId{PROTOCOL_ID};
  std::uint8_t version{PROTOCOL_VERSION};
  std::uint8_t flags{0};
  std::uint64_t sessionToken{NO_SESSION_TOKEN};
  std::uint16_t sequence{0};
  std::uint16_t acknowledgedSequence{0};
  std::uint32_t acknowledgementBits{0};

  bool operator==(const PacketHeader&) const = default;
};

}  // namespace rtype::network
