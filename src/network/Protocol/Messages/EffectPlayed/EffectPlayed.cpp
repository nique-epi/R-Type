#include "EffectPlayed.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "ContentName.hpp"
#include "EntityIdConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void validate(const EffectPlayed& message) {
  if (message.entityId == identifiers::NO_ENTITY_ID) {
    throw InvalidFieldValueException(ENTITY_ID_FIELD, message.entityId);
  }
  if (!identifiers::isContentName(message.effect)) {
    throw InvalidContentNameException(EFFECT_FIELD, message.effect.size());
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const EffectPlayed& message) {
  validate(message);
  writer.writeUint32(message.entityId);
  writer.writeString(message.effect);
}

EffectPlayed decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<EffectPlayed> type) {
  ByteReader reader{payload};
  EffectPlayed message;
  message.entityId = reader.readUint32();
  message.effect = reader.readString();
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
