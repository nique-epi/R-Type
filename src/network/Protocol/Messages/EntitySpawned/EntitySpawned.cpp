#include "EntitySpawned.hpp"
#include <cmath>
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
#include "WorldState.hpp"

namespace rtype::network {

namespace {

bool isListed(WireEntityType entityType) {
  switch (entityType) {
    case WireEntityType::PlayerShip:
    case WireEntityType::PlayerMissile:
    case WireEntityType::Bydo:
    case WireEntityType::EnemyMissile:
      return true;
  }
  return false;
}

void validate(const EntitySpawned& message) {
  if (message.entityId == identifiers::NO_ENTITY_ID) {
    throw InvalidFieldValueException(ENTITY_ID_FIELD, message.entityId);
  }
  if (!isListed(message.entityType)) {
    throw InvalidFieldValueException(
        ENTITY_TYPE_FIELD, static_cast<std::uint8_t>(message.entityType));
  }
  if (std::isnan(message.x) || std::isnan(message.y)) {
    throw InvalidCoordinateException();
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const EntitySpawned& message) {
  validate(message);
  writer.writeUint32(message.entityId);
  writer.writeUint8(static_cast<std::uint8_t>(message.entityType));
  writeCoordinate(writer, message.x);
  writeCoordinate(writer, message.y);
}

EntitySpawned decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<EntitySpawned> type) {
  ByteReader reader{payload};
  const EntitySpawned message{
      .entityId = reader.readUint32(),
      .entityType = static_cast<WireEntityType>(reader.readUint8()),
      .x = readCoordinate(reader),
      .y = readCoordinate(reader)};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
