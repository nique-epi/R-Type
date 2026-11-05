#include "EntitySpawned.hpp"
#include <cmath>
#include <cstddef>
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

void validate(const EntitySpawned& message) {
  if (message.entityId == identifiers::NO_ENTITY_ID) {
    throw InvalidFieldValueException(ENTITY_ID_FIELD, message.entityId);
  }
  if (message.entityType == NO_ENTITY_TYPE) {
    throw InvalidFieldValueException(ENTITY_TYPE_FIELD, message.entityType);
  }
  if (std::isnan(message.x) || std::isnan(message.y)) {
    throw InvalidCoordinateException();
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const EntitySpawned& message) {
  validate(message);
  writer.writeUint32(message.entityId);
  writer.writeUint8(message.entityType);
  writeCoordinate(writer, message.x);
  writeCoordinate(writer, message.y);
}

EntitySpawned decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<EntitySpawned> type) {
  ByteReader reader{payload};
  const EntitySpawned message{.entityId = reader.readUint32(),
                              .entityType = reader.readUint8(),
                              .x = readCoordinate(reader),
                              .y = readCoordinate(reader)};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
