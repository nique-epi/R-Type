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
  writer.writeUint8(player.weapon);
  writer.writeUint8(player.weaponLevel);
}

void writeUnpackedEntity(ByteWriter& writer, const EntityState& entity) {
  writer.writeUint32(entity.entityId);
  writer.writeUint8(entity.entityType);
  writer.writeFloat(entity.x);
  writer.writeFloat(entity.y);
  writer.writeUint8(entity.look);
}

/** @brief Records of an older payload: how many, the bytes of each, and the
 * bytes of 0 a later version writes after each. */
struct VersionRecords {
  std::size_t count{0};
  std::size_t size{0};
  std::size_t added{0};
};

/** @brief Appends to @p converted the records that start @p records, of the
 * count and size @p shape gives, each followed by its added bytes of 0. */
void appendRecords(std::vector<std::byte>& converted,
                   std::span<const std::byte> records, VersionRecords shape) {
  for (std::size_t record = 0; record < shape.count; ++record) {
    const std::span<const std::byte> fields =
        records.subspan(record * shape.size, shape.size);
    converted.insert(converted.end(), fields.begin(), fields.end());
    converted.insert(converted.end(), shape.added, std::byte{0});
  }
}

/** @returns @p payload, a WorldState of protocol version 15, with a weapon 0
 * of level 0 after each of its players, as version 24 writes them, and a look
 * 0 after each of its entities, as version 16 writes them. */
std::vector<std::byte> inTheCurrentLayout(std::span<const std::byte> payload) {
  const auto playerCount =
      std::to_integer<std::size_t>(payload[UINT32_WIRE_SIZE]);
  const std::size_t playersAt = UINT32_WIRE_SIZE + UINT8_WIRE_SIZE;
  const std::size_t countAt =
      playersAt + (playerCount * VERSION_15_PLAYER_BYTES);
  const auto entityCount = std::to_integer<std::size_t>(payload[countAt]);
  std::vector<std::byte> converted(payload.begin(),
                                   payload.begin() + playersAt);
  appendRecords(converted, payload.subspan(playersAt),
                VersionRecords{.count = playerCount,
                               .size = VERSION_15_PLAYER_BYTES,
                               .added = PLAYER_WEAPON_BYTES});
  converted.push_back(payload[countAt]);
  appendRecords(converted, payload.subspan(countAt + 1),
                VersionRecords{.count = entityCount,
                               .size = VERSION_15_ENTITY_BYTES,
                               .added = ENTITY_LOOK_BYTES});
  return converted;
}

}  // namespace

WorldState capturedWorldState(std::span<const std::uint8_t> body) {
  return WorldStateCodec::decode(
      inTheCurrentLayout(std::as_bytes(body).subspan(MESSAGE_PREFIX_SIZE)));
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
