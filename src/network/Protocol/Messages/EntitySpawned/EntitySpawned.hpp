#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief An entity appears: its network id, the number of its type in the
 * table of the ConnectionAccepted of the session, and where its center is, in
 * logical units. A lost one is corrected by the next WorldState, which creates
 * the entity with its appearance effect.
 */
struct EntitySpawned {
  static constexpr MessageType TYPE = MessageType::EntitySpawned;

  std::uint32_t entityId{};
  std::uint8_t entityType{};
  float x{};
  float y{};

  bool operator==(const EntitySpawned&) const = default;
};

/**
 * @brief Writes the entity id, the entity type, then the position.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * entity type is NO_ENTITY_TYPE.
 * @throws InvalidCoordinateException when a coordinate is NaN. Nothing is
 * written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const EntitySpawned& message);

/**
 * @brief Reads an EntitySpawned from the whole of @p payload. Any type but
 * NO_ENTITY_TYPE is read: what it names is for the receiver to look up in the
 * table of the ConnectionAccepted of its session.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * entity type is NO_ENTITY_TYPE.
 * @throws TrailingBytesException when bytes follow the position.
 */
[[nodiscard]] EntitySpawned decodePayload(
    std::span<const std::byte> payload, std::type_identity<EntitySpawned> type);

}  // namespace rtype::network
