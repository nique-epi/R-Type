#include "WorldStateCodec.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"
#include "WorldState.hpp"
#include "WorldStateCodecConstants.hpp"

namespace rtype::network {

namespace {

void writePlayers(ByteWriter& writer, const std::vector<PlayerState>& players) {
  writer.writeUint8(static_cast<std::uint8_t>(players.size()));
  for (const PlayerState& player : players) {
    writer.writeUint8(player.playerNumber);
    writer.writeUint32(player.shipEntityId);
    writer.writeUint8(player.lives);
    writer.writeUint32(player.score);
    writeBool(writer, player.invulnerable);
  }
}

void writeEntity(ByteWriter& writer, const EntityState& entity) {
  writer.writeUint32(entity.entityId);
  writer.writeUint8(static_cast<std::uint8_t>(entity.entityType));
  writeCoordinate(writer, entity.x);
  writeCoordinate(writer, entity.y);
}

std::vector<std::byte> encodePart(const WorldState& state,
                                  std::size_t firstEntity,
                                  std::size_t entityCount) {
  ByteWriter writer;
  writer.writeUint32(state.tick);
  writePlayers(writer, state.players);
  writer.writeUint8(static_cast<std::uint8_t>(entityCount));
  for (std::size_t index = firstEntity; index < firstEntity + entityCount;
       ++index) {
    writeEntity(writer, state.entities[index]);
  }
  return writer.bytes();
}

std::vector<PlayerState> readPlayers(ByteReader& reader) {
  const std::uint8_t playerCount = reader.readUint8();
  if (playerCount > MAXIMUM_PLAYERS) {
    throw MalformedMessageException("a world state lists " +
                                    std::to_string(playerCount) + " players");
  }
  std::vector<PlayerState> players;
  players.reserve(playerCount);
  for (std::uint8_t index = 0; index < playerCount; ++index) {
    players.push_back(PlayerState{.playerNumber = reader.readUint8(),
                                  .shipEntityId = reader.readUint32(),
                                  .lives = reader.readUint8(),
                                  .score = reader.readUint32(),
                                  .invulnerable = readBool(reader)});
  }
  return players;
}

WireEntityType readEntityType(ByteReader& reader) {
  const std::uint8_t type = reader.readUint8();
  if (type < FIRST_WIRE_ENTITY_TYPE || type > LAST_WIRE_ENTITY_TYPE) {
    throw MalformedMessageException("unknown entity type " +
                                    std::to_string(type));
  }
  return static_cast<WireEntityType>(type);
}

EntityState readEntity(ByteReader& reader) {
  return EntityState{.entityId = reader.readUint32(),
                     .entityType = readEntityType(reader),
                     .x = readCoordinate(reader),
                     .y = readCoordinate(reader)};
}

std::vector<EntityState> readEntities(ByteReader& reader) {
  const std::uint8_t entityCount = reader.readUint8();
  std::vector<EntityState> entities;
  entities.reserve(entityCount);
  for (std::uint8_t index = 0; index < entityCount; ++index) {
    entities.push_back(readEntity(reader));
  }
  return entities;
}

}  // namespace

std::size_t WorldStateCodec::maximumEntitiesPerMessage(
    std::size_t playerCount) {
  const std::size_t fixedSize =
      WORLD_STATE_FIXED_SIZE + (playerCount * PLAYER_STATE_WIRE_SIZE);
  return (WORLD_STATE_PAYLOAD_LIMIT - fixedSize) / ENTITY_STATE_WIRE_SIZE;
}

std::vector<std::vector<std::byte>> WorldStateCodec::encode(
    const WorldState& state) {
  if (state.players.size() > MAXIMUM_PLAYERS) {
    throw TooManyPlayersException(state.players.size(), MAXIMUM_PLAYERS);
  }
  const std::size_t entitiesPerMessage =
      maximumEntitiesPerMessage(state.players.size());
  std::vector<std::vector<std::byte>> payloads;
  for (std::size_t first = 0; first < state.entities.size();
       first += entitiesPerMessage) {
    const std::size_t count =
        std::min(entitiesPerMessage, state.entities.size() - first);
    payloads.push_back(encodePart(state, first, count));
  }
  if (payloads.empty()) {
    payloads.push_back(encodePart(state, 0, 0));
  }
  return payloads;
}

WorldState WorldStateCodec::decode(std::span<const std::byte> payload) {
  ByteReader reader{payload};
  WorldState state{.tick = reader.readUint32(),
                   .players = readPlayers(reader),
                   .entities = readEntities(reader)};
  if (reader.remaining() != 0) {
    throw MalformedMessageException(std::to_string(reader.remaining()) +
                                    " byte(s) after the last entity");
  }
  return state;
}

}  // namespace rtype::network
