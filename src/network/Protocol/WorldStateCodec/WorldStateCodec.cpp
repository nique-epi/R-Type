#include "WorldStateCodec.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"
#include "WorldState.hpp"
#include "WorldStateFields.hpp"

namespace rtype::network {

namespace {

std::vector<std::byte> encodePart(const WorldState& state,
                                  std::size_t firstEntity,
                                  std::size_t entityCount) {
  ByteWriter writer;
  writer.writeUint32(state.tick);
  writePlayers(writer, std::span<const PlayerState>{state.players});
  writer.writeUint8(static_cast<std::uint8_t>(entityCount));
  for (std::size_t index = firstEntity; index < firstEntity + entityCount;
       ++index) {
    writeEntity(writer, state.entities[index]);
  }
  return writer.bytes();
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
  requireFullyRead(reader);
  requireValidPlayers(std::span<const PlayerState>{state.players});
  return state;
}

WorldState decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<WorldState> type) {
  return WorldStateCodec::decode(payload);
}

}  // namespace rtype::network
