#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "EntityTypeEntry.hpp"
#include "MessageType.hpp"
#include "ShipGun.hpp"

namespace rtype::network {

/**
 * @brief The server gives the client a place in the game: the number of the
 * player, which also picks its color, and the entity types of the server. The
 * header of the datagram carries the session token the client copies from then
 * on.
 *
 * Type number n (from 1) is types[n - 1]: the type byte of EntitySpawned,
 * WorldState and DeltaWorldState names these entries for the whole session.
 * The table is the same for every player of a server and does not change
 * while it runs, so the acceptance repeated to a player holds it again.
 */
struct ConnectionAccepted {
  static constexpr MessageType TYPE = MessageType::ConnectionAccepted;

  std::uint8_t playerNumber{};
  std::vector<EntityTypeEntry> types;
  ShipGun shipGun{};

  bool operator==(const ConnectionAccepted&) const = default;
};

/**
 * @brief Writes the player number, the number of types, then each type: its
 * side, its name and the size of its box; then the gun of the ships.
 * @throws InvalidFieldValueException when the player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS (playerNumber), there is no type or
 * more than MAXIMUM_ENTITY_TYPES (typeCount), or a side is not one of
 * EntitySide (side).
 * @throws InvalidTypeNameException when a name is not
 * MINIMUM_ENTITY_TYPE_NAME_LENGTH to MAXIMUM_ENTITY_TYPE_NAME_LENGTH bytes of
 * lowercase letters, digits and underscores.
 * @throws InvalidFieldValueException when the gun names a type past the table
 * (shipType, projectileType), a projectile that is not a missile of a player
 * fired by a ship of a player, or fires it with intervalTicks 0
 * (projectileType, intervalTicks), or when a velocity is not finite
 * (velocity). Nothing is written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const ConnectionAccepted& message);

/**
 * @brief Reads a ConnectionAccepted from the whole of @p payload. The number
 * of types is checked before any type is read.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException for a player number, a number of types
 * or a side the protocol does not allow.
 * @throws InvalidTypeNameException for a name the protocol does not allow.
 * @throws InvalidFieldValueException for a gun the protocol does not allow.
 * @throws TrailingBytesException when bytes follow the gun.
 */
[[nodiscard]] ConnectionAccepted decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ConnectionAccepted> type);

}  // namespace rtype::network
