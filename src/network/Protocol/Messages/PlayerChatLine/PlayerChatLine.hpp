#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "ChatLine.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief A line of chat the server relays to every player of a game, its writer
 * included, guaranteed and ordered: the writer learns from it that the line
 * went through.
 */
struct PlayerChatLine {
  static constexpr MessageType TYPE = MessageType::PlayerChatLine;

  /** @brief The line as its writer sent it, its number included. */
  ChatLine line;
  /** @brief The player who wrote it, FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS;
   * the server writes it, never the client. */
  std::uint8_t playerNumber{};

  bool operator==(const PlayerChatLine&) const = default;
};

/**
 * @brief Writes the line, then the number of its player.
 * @throws InvalidChatTextException when the text breaks isValidChatText().
 * @throws InvalidFieldValueException when the player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS. Nothing is written when either is
 * raised.
 */
void encodePayload(ByteWriter& writer, const PlayerChatLine& message);

/**
 * @brief Reads a PlayerChatLine from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidChatTextException or InvalidFieldValueException for a value
 * the protocol does not allow.
 * @throws TrailingBytesException when bytes follow the player number.
 */
[[nodiscard]] PlayerChatLine decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<PlayerChatLine> type);

}  // namespace rtype::network
