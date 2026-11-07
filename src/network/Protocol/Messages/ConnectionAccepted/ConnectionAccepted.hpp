#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "ContentAnnouncement.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The server gives the client a place in the game: the number of the
 * player, which also picks its color, and the content the game plays with.
 * The header of the datagram carries the session token the client copies from
 * then on.
 *
 * The entity types are those of the sheets of the content pack, numbered from
 * 1 in the order of their names: the type byte of EntitySpawned, WorldState
 * and DeltaWorldState names them for the whole session. The pack is the same
 * for every player of a server and does not change while it runs, so the
 * acceptance repeated to a player announces it again.
 */
struct ConnectionAccepted {
  static constexpr MessageType TYPE = MessageType::ConnectionAccepted;

  std::uint8_t playerNumber{};
  ContentAnnouncement content;

  bool operator==(const ConnectionAccepted&) const = default;
};

/**
 * @brief Writes the player number, then the fingerprint of the content pack
 * (CONTENT_FINGERPRINT_BYTES), its size (u32) and the port it is served on
 * (u16): CONNECTION_ACCEPTED_PAYLOAD_SIZE bytes.
 * @throws InvalidFieldValueException when the player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS (playerNumber), or the size is above
 * MAXIMUM_CONTENT_PACK_BYTES (contentSize); nothing is written then.
 */
void encodePayload(ByteWriter& writer, const ConnectionAccepted& message);

/**
 * @brief Reads a ConnectionAccepted from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException for a player number or a size the
 * protocol does not allow.
 * @throws TrailingBytesException when bytes follow the port.
 */
[[nodiscard]] ConnectionAccepted decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ConnectionAccepted> type);

}  // namespace rtype::network
