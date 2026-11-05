#include "ReliabilityFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "EncodedMessage.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkTestConstants.hpp"
#include "OrderedMessage.hpp"
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
OrderedMessage envelopeOf(std::uint16_t position, std::uint16_t oldestKept) {
  return OrderedMessage{
      .number = position,
      .firstKept = oldestKept,
      .innerType = static_cast<std::uint8_t>(MessageType::GameEvent),
      .payload = numberedMessage(position).payload};
}

std::vector<std::uint16_t> carriedNumbers(
    const std::vector<EncodedMessage>& messages) {
  std::vector<std::uint16_t> numbers;
  numbers.reserve(messages.size());
  for (const EncodedMessage& message : messages) {
    numbers.push_back(numberOf(message));
  }
  return numbers;
}

OrderedMessage decodedEnvelope(const EncodedMessage& message) {
  return decodeMessage<OrderedMessage>(message.payload);
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
