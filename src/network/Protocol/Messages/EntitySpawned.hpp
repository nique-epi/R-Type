#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "WorldState.hpp"

namespace rtype::network {

/**
 * @brief An entity appears: its network id, what it is, and where its center
 * is, in logical units. A lost one is corrected by the next WorldState, which
 * creates the entity with its appearance effect.
 */
struct EntitySpawned {
  static constexpr MessageType TYPE = MessageType::EntitySpawned;

  std::uint32_t entityId;
  WireEntityType entityType;
  float x;
  float y;

  bool operator==(const EntitySpawned&) const = default;
};

/**
 * @brief Writes the entity id, the entity type, then the position.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * entity type is not one of WireEntityType.
 * @throws InvalidCoordinateException when a coordinate is NaN. Nothing is
 * written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const EntitySpawned& message);

/**
 * @brief Reads an EntitySpawned from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * entity type is not one of WireEntityType.
 * @throws TrailingBytesException when bytes follow the position.
 */
[[nodiscard]] EntitySpawned decodePayload(
    std::span<const std::byte> payload, std::type_identity<EntitySpawned> type);

}  // namespace rtype::network
