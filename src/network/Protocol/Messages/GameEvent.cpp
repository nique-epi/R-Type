#include "GameEvent.hpp"
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

void requireEntityWithoutPlayer(const GameEvent& message) {
  if (message.playerNumber != NO_PLAYER_NUMBER) {
    throw InvalidFieldValueException(PLAYER_NUMBER_FIELD, message.playerNumber);
  }
  if (message.entityId == NO_ENTITY_ID) {
    throw InvalidFieldValueException(ENTITY_ID_FIELD, message.entityId);
  }
}

void requirePlayerWithoutEntity(const GameEvent& message) {
  requireValidPlayerNumber(message.playerNumber);
  if (message.entityId != NO_ENTITY_ID) {
    throw InvalidFieldValueException(ENTITY_ID_FIELD, message.entityId);
  }
}

void validate(const GameEvent& message) {
  switch (message.kind) {
    case GameEventKind::EntityDamaged:
      requireEntityWithoutPlayer(message);
      return;
    case GameEventKind::PlayerHit:
    case GameEventKind::PlayerEliminated:
      requirePlayerWithoutEntity(message);
      return;
  }
  throw InvalidFieldValueException(KIND_FIELD,
                                   static_cast<std::uint8_t>(message.kind));
}

}  // namespace

void encodePayload(ByteWriter& writer, const GameEvent& message) {
  validate(message);
  writer.writeUint16(message.eventNumber);
  writer.writeUint8(static_cast<std::uint8_t>(message.kind));
  writer.writeUint8(message.playerNumber);
  writer.writeUint32(message.entityId);
}

GameEvent decodePayload(std::span<const std::byte> payload,
                        [[maybe_unused]] std::type_identity<GameEvent> type) {
  ByteReader reader{payload};
  const GameEvent message{
      .eventNumber = reader.readUint16(),
      .kind = static_cast<GameEventKind>(reader.readUint8()),
      .playerNumber = reader.readUint8(),
      .entityId = reader.readUint32()};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
