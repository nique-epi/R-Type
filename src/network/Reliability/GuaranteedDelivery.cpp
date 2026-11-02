#include "GuaranteedDelivery.hpp"
#include <cstddef>
#include <cstdint>
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

bool acknowledges(const PacketHeader& header, std::uint16_t sequence) {
  if ((header.flags & ACKNOWLEDGES_FLAG) == 0) {
    return false;
  }
  const auto distance =
      static_cast<std::uint16_t>(header.acknowledgedSequence - sequence);
  if (distance == 0) {
    return true;
  }
  if (std::size_t{distance} > ACKNOWLEDGEMENT_BIT_COUNT) {
    return false;
  }
  return ((header.acknowledgementBits >> (distance - 1U)) & 1U) != 0;
}

bool holdsGuaranteedMessages(const PacketHeader& header) {
  return (header.flags & GUARANTEED_FLAG) != 0;
}

void markGuaranteed(PacketHeader& header) {
  header.flags = static_cast<std::uint8_t>(header.flags | GUARANTEED_FLAG);
}

}  // namespace rtype::network
