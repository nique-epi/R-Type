#include "ConnectionAccepted.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "EntitySide.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeEntry.hpp"
#include "EntityTypeName.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "ShipGun.hpp"
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

/** @returns Whether @p number is 0 or the number of a type of @p types. */
bool namesNoTypeOrOneOf(std::uint8_t number,
                        const std::vector<EntityTypeEntry>& types) {
  return number <= types.size();
}

/** @returns Whether type @p number of @p types, a number of the table, is of
 * side @p side. */
bool isOfSide(std::uint8_t number, const std::vector<EntityTypeEntry>& types,
              EntitySide side) {
  return types[number - 1].side == side;
}

/** @throws InvalidFieldValueException naming shipType or projectileType when
 * one names a type past @p types, see encodePayload(). */
void requireTypesOfTheTable(const ShipGun& gun,
                            const std::vector<EntityTypeEntry>& types) {
  if (!namesNoTypeOrOneOf(gun.shipType, types)) {
    throw InvalidFieldValueException(SHIP_TYPE_FIELD, gun.shipType);
  }
  if (!namesNoTypeOrOneOf(gun.projectileType, types)) {
    throw InvalidFieldValueException(PROJECTILE_TYPE_FIELD, gun.projectileType);
  }
}

/** @throws InvalidFieldValueException naming projectileType or intervalTicks
 * when a predicted missile is not one a ship of a player fires, see
 * encodePayload(). */
void requirePredictableMissile(const ShipGun& gun,
                               const std::vector<EntityTypeEntry>& types) {
  if (gun.projectileType == identifiers::NO_ENTITY_TYPE) {
    return;
  }
  if (gun.shipType == identifiers::NO_ENTITY_TYPE ||
      !isOfSide(gun.shipType, types, EntitySide::Player) ||
      !isOfSide(gun.projectileType, types, EntitySide::PlayerMissile)) {
    throw InvalidFieldValueException(PROJECTILE_TYPE_FIELD, gun.projectileType);
  }
  if (gun.intervalTicks == 0) {
    throw InvalidFieldValueException(INTERVAL_TICKS_FIELD, gun.intervalTicks);
  }
}

/** @throws InvalidCoordinateException when a velocity of @p gun is not
 * finite. */
void requireFiniteVelocity(const ShipGun& gun) {
  if (!std::isfinite(gun.velocityX) || !std::isfinite(gun.velocityY)) {
    throw InvalidCoordinateException();
  }
}

void validate(const ConnectionAccepted& message) {
  requireValidPlayerNumber(message.playerNumber);
  requireValidTypeCount(message.types.size());
  for (const EntityTypeEntry& entry : message.types) {
    requireValidSide(entry.side);
    requireValidTypeName(entry.name);
  }
  requireTypesOfTheTable(message.shipGun, message.types);
  requirePredictableMissile(message.shipGun, message.types);
  requireFiniteVelocity(message.shipGun);
}

EntityTypeEntry readEntry(ByteReader& reader) {
  return EntityTypeEntry{.side = static_cast<EntitySide>(reader.readUint8()),
                         .name = reader.readString(),
                         .hitboxWidth = reader.readUint16(),
                         .hitboxHeight = reader.readUint16()};
}

void writeEntry(ByteWriter& writer, const EntityTypeEntry& entry) {
  writer.writeUint8(static_cast<std::uint8_t>(entry.side));
  writer.writeString(entry.name);
  writer.writeUint16(entry.hitboxWidth);
  writer.writeUint16(entry.hitboxHeight);
}

ShipGun readShipGun(ByteReader& reader) {
  return ShipGun{.shipType = reader.readUint8(),
                 .projectileType = reader.readUint8(),
                 .intervalTicks = reader.readUint16(),
                 .firstShotTicks = reader.readUint16(),
                 .velocityX = reader.readFloat(),
                 .velocityY = reader.readFloat()};
}

void writeShipGun(ByteWriter& writer, const ShipGun& gun) {
  writer.writeUint8(gun.shipType);
  writer.writeUint8(gun.projectileType);
  writer.writeUint16(gun.intervalTicks);
  writer.writeUint16(gun.firstShotTicks);
  writer.writeFloat(gun.velocityX);
  writer.writeFloat(gun.velocityY);
}

}  // namespace

void encodePayload(ByteWriter& writer, const ConnectionAccepted& message) {
  validate(message);
  writer.writeUint8(message.playerNumber);
  writer.writeUint8(static_cast<std::uint8_t>(message.types.size()));
  for (const EntityTypeEntry& entry : message.types) {
    writeEntry(writer, entry);
  }
  writeShipGun(writer, message.shipGun);
}

ConnectionAccepted decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionAccepted> type) {
  ByteReader reader{payload};
  ConnectionAccepted message{
      .playerNumber = reader.readUint8(), .types = {}, .shipGun = {}};
  const std::uint8_t typeCount = reader.readUint8();
  requireValidTypeCount(typeCount);
  message.types.reserve(typeCount);
  for (std::uint8_t index = 0; index < typeCount; ++index) {
    message.types.push_back(readEntry(reader));
  }
  message.shipGun = readShipGun(reader);
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
