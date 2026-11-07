#include "PackingMeasurementFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "ByteWriter.hpp"
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
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
}

}  // namespace

WorldState capturedWorldState(std::span<const std::uint8_t> body) {
  return WorldStateCodec::decode(
      std::as_bytes(body).subspan(MESSAGE_PREFIX_SIZE));
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
