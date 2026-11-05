#include "EnemyAction.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "EntityIdConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

bool isListed(EnemyActionPhase phase) {
  switch (phase) {
    case EnemyActionPhase::Signal:
    case EnemyActionPhase::Strike:
    case EnemyActionPhase::Recover:
    case EnemyActionPhase::Enraged:
      return true;
  }
  return false;
}

void validate(const EnemyAction& message) {
  if (message.entityId == identifiers::NO_ENTITY_ID) {
    throw InvalidFieldValueException(ENTITY_ID_FIELD, message.entityId);
  }
  if (!isListed(message.phase)) {
    throw InvalidFieldValueException(PHASE_FIELD,
                                     static_cast<std::uint8_t>(message.phase));
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const EnemyAction& message) {
  validate(message);
  writer.writeUint32(message.entityId);
  writer.writeUint8(message.attack);
  writer.writeUint8(static_cast<std::uint8_t>(message.phase));
}

EnemyAction decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<EnemyAction> type) {
  ByteReader reader{payload};
  const EnemyAction message{
      .entityId = reader.readUint32(),
      .attack = reader.readUint8(),
      .phase = static_cast<EnemyActionPhase>(reader.readUint8())};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
