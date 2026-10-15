#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The server gives the client a place in the game: the number of the
 * player, which also picks its color. The header of the datagram carries the
 * session token the client copies from then on.
 */
struct ConnectionAccepted {
  static constexpr MessageType TYPE = MessageType::ConnectionAccepted;

  std::uint8_t playerNumber;

  bool operator==(const ConnectionAccepted&) const = default;
};

/**
 * @brief Writes the player number.
 * @throws InvalidFieldValueException when the player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const ConnectionAccepted& message);

/**
 * @brief Reads a ConnectionAccepted from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is empty.
 * @throws InvalidFieldValueException when the player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS.
 * @throws TrailingBytesException when bytes follow the player number.
 */
[[nodiscard]] ConnectionAccepted decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ConnectionAccepted> type);

}  // namespace rtype::network
