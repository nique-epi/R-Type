#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief An entity appears: its network id, the number of its type in the
 * table of the ConnectionAccepted of the session, where its center is, in
 * logical units, and, for a missile of a player, the player who fired it, the
 * inputNumber of the PlayerInput that fired it and the rank of the missile
 * among the projectiles of that shot, from 0, so that player can tell the
 * missile it predicted, one of a fan included. A lost one is corrected by the
 * next WorldState, which creates the entity with its appearance effect; the
 * server guarantees the one of a missile of a player.
 */
struct EntitySpawned {
  static constexpr MessageType TYPE = MessageType::EntitySpawned;

  std::uint32_t entityId{};
  std::uint8_t entityType{};
  float x{};
  float y{};
  std::uint8_t owner{NO_OWNER};
  std::uint32_t firingInput{NO_FIRING_INPUT};
  std::uint8_t shotRank{0};

  bool operator==(const EntitySpawned&) const = default;
};

/**
 * @brief Writes the entity id, the entity type, the position, the owner, the
 * firing input, then the shot rank.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * entity type is NO_ENTITY_TYPE, or the owner is above MAXIMUM_PLAYERS.
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
 * entity type is NO_ENTITY_TYPE, or the owner is above MAXIMUM_PLAYERS.
 * @throws TrailingBytesException when bytes follow the shot rank.
 */
[[nodiscard]] EntitySpawned decodePayload(
    std::span<const std::byte> payload, std::type_identity<EntitySpawned> type);

}  // namespace rtype::network
