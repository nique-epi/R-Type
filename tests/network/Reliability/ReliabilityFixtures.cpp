#include "ReliabilityFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "EncodedMessage.hpp"
#include "MessageType.hpp"
#include "NetworkTestConstants.hpp"
#include "PacketHeader.hpp"

namespace rtype::network::testing {

EncodedMessage numberedMessage(std::uint16_t number) {
  ByteWriter writer;
  writer.writeUint16(number);
  return EncodedMessage{.type = MessageType::GameEvent,
                        .payload = writer.bytes()};
}

std::uint16_t numberOf(const EncodedMessage& message) {
  ByteReader reader{message.payload};
  return reader.readUint16();
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
PacketHeader acknowledgingHeader(std::uint16_t acknowledgedSequence,
                                 std::uint32_t bits) {
  PacketHeader header;
  header.flags = PROTOCOL_ACKNOWLEDGES_FLAG;
  header.acknowledgedSequence = acknowledgedSequence;
  header.acknowledgementBits = bits;
  return header;
}

}  // namespace rtype::network::testing
