#include "EntityDestroyed.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

bool isListed(DestructionReason reason) {
  switch (reason) {
    case DestructionReason::Killed:
    case DestructionReason::Removed:
      return true;
  }
  return false;
}

void validate(const EntityDestroyed& message) {
  if (message.entityId == NO_ENTITY_ID) {
    throw InvalidFieldValueException("entityId", message.entityId);
  }
  if (!isListed(message.reason)) {
    throw InvalidFieldValueException("reason",
                                     static_cast<std::uint8_t>(message.reason));
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const EntityDestroyed& message) {
  validate(message);
  writer.writeUint32(message.entityId);
  writer.writeUint8(static_cast<std::uint8_t>(message.reason));
}

EntityDestroyed decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<EntityDestroyed> type) {
  ByteReader reader{payload};
  const EntityDestroyed message{
      .entityId = reader.readUint32(),
      .reason = static_cast<DestructionReason>(reader.readUint8())};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
