#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief How a player left: Left when it said so, TimedOut when the server
 * heard nothing from it for 5 s. The values are the ones of the protocol
 * page.
 */
enum class DepartureReason : std::uint8_t {
  Left = 1,
  TimedOut = 2,
};

/**
 * @brief Tells the other players that one left. Sent three times, 50 ms
 * apart; a copy naming a player the client no longer lists is ignored.
 */
struct PlayerLeft {
  static constexpr MessageType TYPE = MessageType::PlayerLeft;

  std::uint8_t playerNumber;
  DepartureReason reason;

  bool operator==(const PlayerLeft&) const = default;
};

/**
 * @brief Writes the player number, then the reason.
 * @throws InvalidFieldValueException when the player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS or the reason is not one of
 * DepartureReason; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const PlayerLeft& message);

/**
 * @brief Reads a PlayerLeft from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when the player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS or the reason is not one of
 * DepartureReason.
 * @throws TrailingBytesException when bytes follow the reason.
 */
[[nodiscard]] PlayerLeft decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<PlayerLeft> type);

}  // namespace rtype::network
