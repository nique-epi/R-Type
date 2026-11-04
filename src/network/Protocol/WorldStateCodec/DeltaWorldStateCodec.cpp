#include "DeltaWorldStateCodec.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "DeltaWorldState.hpp"
#include "EntityIdConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"
#include "WorldState.hpp"
#include "WorldStateFields.hpp"

namespace rtype::network {

namespace {

/** @brief The part of the two lists one payload holds, and where the payload
 * stands among the parts of its state. */
struct PartSlices {
  std::size_t firstChanged;
  std::size_t changedCount;
  std::size_t firstRemoved;
  std::size_t removedCount;
  std::size_t partIndex;
  std::size_t partCount;
};

/** @returns The bytes of one payload left for the two lists, next to
 * @p playerCount players. */
std::size_t roomForLists(std::size_t playerCount) {
  return WORLD_STATE_PAYLOAD_LIMIT - DELTA_WORLD_STATE_FIXED_SIZE -
         (playerCount * PLAYER_STATE_WIRE_SIZE);
}

/** @returns How the two lists of @p whole are cut into payloads: each takes
 * changed entities first, then removed ids, until its room is used. Always at
 * least one part, so a state without entities is sent. */
std::vector<PartSlices> slicesOf(const DeltaWorldState& whole) {
  const std::size_t room = roomForLists(whole.players.size());
  std::vector<PartSlices> slices;
  std::size_t nextChanged = 0;
  std::size_t nextRemoved = 0;
  while (slices.empty() || nextChanged < whole.changed.size() ||
         nextRemoved < whole.removed.size()) {
    const std::size_t changedCount = std::min(
        {whole.changed.size() - nextChanged, room / ENTITY_STATE_WIRE_SIZE,
         MAXIMUM_ENTITIES_PER_DELTA_LIST});
    const std::size_t roomLeft = room - (changedCount * ENTITY_STATE_WIRE_SIZE);
    const std::size_t removedCount = std::min(
        {whole.removed.size() - nextRemoved,
         roomLeft / REMOVED_ENTITY_WIRE_SIZE, MAXIMUM_ENTITIES_PER_DELTA_LIST});
    slices.push_back(PartSlices{.firstChanged = nextChanged,
                                .changedCount = changedCount,
                                .firstRemoved = nextRemoved,
                                .removedCount = removedCount,
                                .partIndex = slices.size(),
                                .partCount = 0});
    nextChanged += changedCount;
    nextRemoved += removedCount;
  }
  for (PartSlices& part : slices) {
    part.partCount = slices.size();
  }
  return slices;
}

void writePartLists(ByteWriter& writer, const DeltaWorldState& whole,
                    const PartSlices& slices) {
  writer.writeUint8(static_cast<std::uint8_t>(slices.changedCount));
  for (std::size_t index = slices.firstChanged;
       index < slices.firstChanged + slices.changedCount; ++index) {
    writeEntity(writer, whole.changed[index]);
  }
  writer.writeUint8(static_cast<std::uint8_t>(slices.removedCount));
  for (std::size_t index = slices.firstRemoved;
       index < slices.firstRemoved + slices.removedCount; ++index) {
    writer.writeUint32(whole.removed[index]);
  }
}

std::vector<std::byte> encodePart(const DeltaWorldState& whole,
                                  const PartSlices& slices) {
  ByteWriter writer;
  writer.writeUint32(whole.tick);
  writer.writeUint8(whole.baseDistance);
  writer.writeUint8(static_cast<std::uint8_t>(slices.partIndex));
  writer.writeUint8(static_cast<std::uint8_t>(slices.partCount));
  writePlayers(writer, std::span<const PlayerState>{whole.players});
  writePartLists(writer, whole, slices);
  return writer.bytes();
}

/** @throws InvalidFieldValueException when a field of the header of @p part
 * holds a value the protocol refuses. */
void validateHeader(const DeltaWorldState& part) {
  if (part.baseDistance > MAXIMUM_REFERENCE_DISTANCE) {
    throw InvalidFieldValueException(BASE_DISTANCE_FIELD, part.baseDistance);
  }
  if (part.partCount == 0 || part.partCount > MAXIMUM_DELTA_PARTS) {
    throw InvalidFieldValueException(PART_COUNT_FIELD, part.partCount);
  }
  if (part.partIndex >= part.partCount) {
    throw InvalidFieldValueException(PART_INDEX_FIELD, part.partIndex);
  }
}

std::vector<std::uint32_t> readRemoved(ByteReader& reader) {
  const std::uint8_t removedCount = reader.readUint8();
  std::vector<std::uint32_t> removed;
  removed.reserve(removedCount);
  for (std::uint8_t index = 0; index < removedCount; ++index) {
    const std::uint32_t entityId = reader.readUint32();
    if (entityId == identifiers::NO_ENTITY_ID) {
      throw InvalidFieldValueException(ENTITY_ID_FIELD, entityId);
    }
    removed.push_back(entityId);
  }
  return removed;
}

}  // namespace

std::vector<std::vector<std::byte>> DeltaWorldStateCodec::encode(
    const DeltaWorldState& whole) {
  if (whole.players.size() > MAXIMUM_PLAYERS) {
    throw TooManyPlayersException(whole.players.size(), MAXIMUM_PLAYERS);
  }
  const std::vector<PartSlices> slices = slicesOf(whole);
  if (slices.size() > MAXIMUM_DELTA_PARTS) {
    throw TooManyDeltaPartsException(slices.size(), MAXIMUM_DELTA_PARTS);
  }
  std::vector<std::vector<std::byte>> payloads;
  payloads.reserve(slices.size());
  for (const PartSlices& part : slices) {
    payloads.push_back(encodePart(whole, part));
  }
  return payloads;
}

DeltaWorldState DeltaWorldStateCodec::decode(
    std::span<const std::byte> payload) {
  ByteReader reader{payload};
  DeltaWorldState part{.tick = reader.readUint32(),
                       .baseDistance = reader.readUint8(),
                       .partIndex = reader.readUint8(),
                       .partCount = reader.readUint8(),
                       .players = {},
                       .changed = {},
                       .removed = {}};
  validateHeader(part);
  part.players = readPlayers(reader);
  part.changed = readEntities(reader);
  part.removed = readRemoved(reader);
  requireFullyRead(reader);
  requireValidPlayers(std::span<const PlayerState>{part.players});
  return part;
}

DeltaWorldState decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<DeltaWorldState> type) {
  return DeltaWorldStateCodec::decode(payload);
}

}  // namespace rtype::network
