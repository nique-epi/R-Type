#include "OrderedMessage.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "SequenceOrder.hpp"

namespace rtype::network {

namespace {

void validate(const OrderedMessage& message) {
  if (message.innerType ==
      static_cast<std::uint8_t>(MessageType::OrderedMessage)) {
    throw InvalidFieldValueException(INNER_TYPE_FIELD, message.innerType);
  }
  if (isNewerSequence(message.firstKept, message.number)) {
    throw InvalidFieldValueException(FIRST_KEPT_FIELD, message.firstKept);
  }
  if (message.payload.size() > ORDERED_PAYLOAD_LIMIT) {
    throw InvalidFieldValueException(CARRIED_PAYLOAD_SIZE_FIELD,
                                     message.payload.size());
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const OrderedMessage& message) {
  validate(message);
  writer.writeUint16(message.number);
  writer.writeUint16(message.firstKept);
  writer.writeUint8(message.innerType);
  writer.writeBytes(message.payload);
}

OrderedMessage decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<OrderedMessage> type) {
  ByteReader reader{payload};
  OrderedMessage message{.number = reader.readUint16(),
                         .firstKept = reader.readUint16(),
                         .innerType = reader.readUint8(),
                         .payload = {}};
  const std::span<const std::byte> carried =
      reader.readBytes(reader.remaining());
  message.payload.assign(carried.begin(), carried.end());
  validate(message);
  return message;
}

}  // namespace rtype::network
