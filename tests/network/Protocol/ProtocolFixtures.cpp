#include "ProtocolFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include "ByteSequence.hpp"
#include "EntitySpawned.hpp"
#include "PacketHeader.hpp"
#include "ProtocolTestConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFixtures.hpp"

namespace rtype::network::testing {

PacketHeader someSessionHeader() {
  PacketHeader header;
  header.sessionToken = SOME_SESSION_TOKEN;
  header.sequence = SOME_SEQUENCE;
  return header;
}

std::vector<std::byte> defaultHeaderBytes() {
  std::vector<std::byte> bytes;
  bytes.reserve(DEFAULT_HEADER_BYTES.size());
  for (const std::uint8_t value : DEFAULT_HEADER_BYTES) {
    bytes.push_back(std::byte{value});
  }
  return bytes;
}

std::vector<std::byte> datagramOf(
    std::initializer_list<unsigned int> messageBytes) {
  std::vector<std::byte> datagram = defaultHeaderBytes();
  const std::vector<std::byte> message = byteSequence(messageBytes);
  datagram.insert(datagram.end(), message.begin(), message.end());
  return datagram;
}

EntitySpawned exampleBydo() {
  return EntitySpawned{.entityId = EXAMPLE_ENTITY_ID,
                       .entityType = WireEntityType::Bydo,
                       .x = EXAMPLE_X,
                       .y = EXAMPLE_Y};
}

}  // namespace rtype::network::testing
