#include "WorldStateFields.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "EntityIdConstants.hpp"
#include "EntityTypeConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"
#include "WorldState.hpp"

namespace rtype::network {

namespace {

/** @returns The motion of a player.
 * @throws InvalidFieldValueException When a bit past PLAYER_MOTION_BITS is
 * set. */
std::uint8_t readMotion(ByteReader& reader) {
  const std::uint8_t motion = reader.readUint8();
  if ((motion & static_cast<std::uint8_t>(~PLAYER_MOTION_BITS)) != 0) {
    throw InvalidFieldValueException(MOTION_FIELD, motion);
  }
  return motion;
}

std::optional<EntityState> readEntity(ByteReader& reader) {
  const std::uint32_t entityId = reader.readUint32();
  const std::uint8_t type = reader.readUint8();
  const float horizontal = readCoordinate(reader);
  const float vertical = readCoordinate(reader);
  const std::uint8_t look = reader.readUint8();
  if (entityId == identifiers::NO_ENTITY_ID) {
    throw InvalidFieldValueException(ENTITY_ID_FIELD, entityId);
  }
  if (type == identifiers::NO_ENTITY_TYPE) {
    return std::nullopt;
  }
  return EntityState{.entityId = entityId,
                     .entityType = type,
                     .x = horizontal,
                     .y = vertical,
                     .look = look};
}

}  // namespace

void writePlayers(ByteWriter& writer, std::span<const PlayerState> players) {
  writer.writeUint8(static_cast<std::uint8_t>(players.size()));
  for (const PlayerState& player : players) {
    writer.writeUint8(player.playerNumber);
    writer.writeUint32(player.shipEntityId);
    writer.writeUint8(player.lives);
    writer.writeUint32(player.score);
    writeBool(writer, player.invulnerable);
    writer.writeUint8(player.motion);
    writer.writeUint32(player.acknowledgedInput);
  }
}

void writeEntity(ByteWriter& writer, const EntityState& entity) {
  writer.writeUint32(entity.entityId);
  writer.writeUint8(entity.entityType);
  writeCoordinate(writer, entity.x);
  writeCoordinate(writer, entity.y);
  writer.writeUint8(entity.look);
}

std::vector<PlayerState> readPlayers(ByteReader& reader) {
  const std::uint8_t playerCount = readPlayerCount(reader);
  std::vector<PlayerState> players;
  players.reserve(playerCount);
  for (std::uint8_t index = 0; index < playerCount; ++index) {
    players.push_back(PlayerState{.playerNumber = reader.readUint8(),
                                  .shipEntityId = reader.readUint32(),
                                  .lives = reader.readUint8(),
                                  .score = reader.readUint32(),
                                  .invulnerable = readBool(reader),
                                  .motion = readMotion(reader),
                                  .acknowledgedInput = reader.readUint32()});
  }
  return players;
}

std::vector<EntityState> readEntities(ByteReader& reader) {
  const std::uint8_t entityCount = reader.readUint8();
  std::vector<EntityState> entities;
  entities.reserve(entityCount);
  for (std::uint8_t index = 0; index < entityCount; ++index) {
    const std::optional<EntityState> entity = readEntity(reader);
    if (entity.has_value()) {
      entities.push_back(*entity);
    }
  }
  return entities;
}

}  // namespace rtype::network
