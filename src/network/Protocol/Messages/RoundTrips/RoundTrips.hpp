#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The round trip of one player, as RoundTrips lists it.
 */
struct PlayerRoundTrip {
  std::uint8_t playerNumber{};
  /** @brief The smoothed round trip, in microseconds. */
  std::uint32_t roundTrip{};
  /** @brief The jitter of the round trip, in microseconds. */
  std::uint32_t jitter{};

  bool operator==(const PlayerRoundTrip&) const = default;
};

/**
 * @brief The round trip the server measured with each player of the game,
 * and its jitter (see RoundTripMeter). The server sends it to each player
 * right after each Heartbeat it sends that player. A player the server
 * measured nothing with yet is not listed.
 */
struct RoundTrips {
  static constexpr MessageType TYPE = MessageType::RoundTrips;

  std::vector<PlayerRoundTrip> players;

  bool operator==(const RoundTrips&) const = default;
};

/**
 * @brief Writes the number of players, then each one: number, round trip,
 * jitter.
 * @throws TooManyPlayersException when there are more than MAXIMUM_PLAYERS.
 * @throws InvalidFieldValueException when a player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS.
 * @throws DuplicatePlayerNumberException when a number appears twice. Nothing
 * is written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const RoundTrips& message);

/**
 * @brief Reads RoundTrips from the whole of @p payload. The number of players
 * is checked before any player is read.
 * @throws TooManyPlayersException when it announces more than MAXIMUM_PLAYERS.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException or DuplicatePlayerNumberException for a
 * player number the protocol does not allow.
 * @throws TrailingBytesException when bytes follow the last player.
 */
[[nodiscard]] RoundTrips decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<RoundTrips> type);

}  // namespace rtype::network
