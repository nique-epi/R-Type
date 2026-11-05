#include "ConnectionAccepted.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "EntitySide.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeEntry.hpp"
#include "EntityTypeName.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

/** @throws InvalidTypeNameException when @p name is not the name of a type,
 * see encodePayload(). */
void requireValidTypeName(std::string_view name) {
  if (!identifiers::isEntityTypeName(name)) {
    throw InvalidTypeNameException(name.size());
  }
}

/** @throws InvalidFieldValueException naming side when @p side is not one of
 * EntitySide. */
void requireValidSide(EntitySide side) {
  const auto value = static_cast<std::uint8_t>(side);
  if (value < static_cast<std::uint8_t>(EntitySide::Player) ||
      value > static_cast<std::uint8_t>(EntitySide::EnemyMissile)) {
    throw InvalidFieldValueException(SIDE_FIELD, value);
  }
}

/** @throws InvalidFieldValueException naming typeCount when @p count is 0 or
 * above MAXIMUM_ENTITY_TYPES. */
void requireValidTypeCount(std::size_t count) {
  if (count == 0 || count > identifiers::MAXIMUM_ENTITY_TYPES) {
    throw InvalidFieldValueException(TYPE_COUNT_FIELD, count);
  }
}

void validate(const ConnectionAccepted& message) {
  requireValidPlayerNumber(message.playerNumber);
  requireValidTypeCount(message.types.size());
  for (const EntityTypeEntry& entry : message.types) {
    requireValidSide(entry.side);
    requireValidTypeName(entry.name);
  }
}

EntityTypeEntry readEntry(ByteReader& reader) {
  return EntityTypeEntry{.side = static_cast<EntitySide>(reader.readUint8()),
                         .name = reader.readString()};
}

}  // namespace

void encodePayload(ByteWriter& writer, const ConnectionAccepted& message) {
  validate(message);
  writer.writeUint8(message.playerNumber);
  writer.writeUint8(static_cast<std::uint8_t>(message.types.size()));
  for (const EntityTypeEntry& entry : message.types) {
    writer.writeUint8(static_cast<std::uint8_t>(entry.side));
    writer.writeString(entry.name);
  }
}

ConnectionAccepted decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionAccepted> type) {
  ByteReader reader{payload};
  ConnectionAccepted message{.playerNumber = reader.readUint8(), .types = {}};
  const std::uint8_t typeCount = reader.readUint8();
  requireValidTypeCount(typeCount);
  message.types.reserve(typeCount);
  for (std::uint8_t index = 0; index < typeCount; ++index) {
    message.types.push_back(readEntry(reader));
  }
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
