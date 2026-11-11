#include "PackingMeasurementFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "ByteWriter.hpp"
#include "PackingMeasurementTestConstants.hpp"
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
#include "SerializationConstants.hpp"
#include "WireTypes.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::network::testing {

namespace {

void writeBitsAsBytes(ByteWriter& writer, std::uint8_t bits,
                      std::size_t count) {
  for (std::size_t bit = 0; bit < count; ++bit) {
    const unsigned int mask = 1U << bit;
    writeBool(writer, (static_cast<unsigned int>(bits) & mask) != 0U);
  }
}

void writeUnpackedPlayer(ByteWriter& writer, const PlayerState& player) {
  writer.writeUint8(player.playerNumber);
  writer.writeUint32(player.shipEntityId);
  writer.writeUint8(player.lives);
  writer.writeUint32(player.score);
  writeBool(writer, player.invulnerable);
  writeBitsAsBytes(writer, player.motion, PLAYER_MOTION_BIT_COUNT);
  writer.writeUint32(player.acknowledgedInput);
}

void writeUnpackedEntity(ByteWriter& writer, const EntityState& entity) {
  writer.writeUint32(entity.entityId);
  writer.writeUint8(entity.entityType);
  writer.writeFloat(entity.x);
  writer.writeFloat(entity.y);
  writer.writeUint8(entity.look);
}

/** @returns @p payload, a WorldState of protocol version 15, with a look 0
 * after each of its entities, as version 16 writes them. */
std::vector<std::byte> withEntityLooks(std::span<const std::byte> payload) {
  const auto playerCount =
      std::to_integer<std::size_t>(payload[UINT32_WIRE_SIZE]);
  const std::size_t countAt = UINT32_WIRE_SIZE + UINT8_WIRE_SIZE +
                              (playerCount * PLAYER_STATE_WIRE_SIZE);
  const auto entityCount = std::to_integer<std::size_t>(payload[countAt]);
  std::vector<std::byte> converted;
  const std::span<const std::byte> head = payload.first(countAt + 1);
  converted.insert(converted.end(), head.begin(), head.end());
  for (std::size_t entity = 0; entity < entityCount; ++entity) {
    const std::span<const std::byte> fields =
        payload.subspan(countAt + 1 + (entity * VERSION_15_ENTITY_BYTES),
                        VERSION_15_ENTITY_BYTES);
    converted.insert(converted.end(), fields.begin(), fields.end());
    converted.push_back(std::byte{0});
  }
  return converted;
}

}  // namespace

WorldState capturedWorldState(std::span<const std::uint8_t> body) {
  return WorldStateCodec::decode(
      withEntityLooks(std::as_bytes(body).subspan(MESSAGE_PREFIX_SIZE)));
}

std::vector<std::byte> unpackedWorldStateBytes(const WorldState& state) {
  ByteWriter writer;
  writer.writeUint32(state.tick);
  writer.writeUint8(static_cast<std::uint8_t>(state.players.size()));
  for (const PlayerState& player : state.players) {
    writeUnpackedPlayer(writer, player);
  }
  writer.writeUint8(static_cast<std::uint8_t>(state.entities.size()));
  for (const EntityState& entity : state.entities) {
    writeUnpackedEntity(writer, entity);
  }
  return writer.bytes();
}

std::vector<std::byte> unpackedPlayerInputBytes(const PlayerInput& input) {
  ByteWriter writer;
  writer.writeUint32(input.inputNumber);
  writeBitsAsBytes(writer, input.actions, PLAYER_ACTION_COUNT);
  writer.writeUint8(static_cast<std::uint8_t>(input.earlierActions.size()));
  for (const std::uint8_t actions : input.earlierActions) {
    writeBitsAsBytes(writer, actions, PLAYER_ACTION_COUNT);
  }
  writer.writeUint32(input.shownTick);
  return writer.bytes();
}

}  // namespace rtype::network::testing
