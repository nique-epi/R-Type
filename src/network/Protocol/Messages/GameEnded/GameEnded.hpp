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
 * @brief The score of one player at the end of a game.
 */
struct PlayerScore {
  std::uint8_t playerNumber;
  std::uint32_t score;

  bool operator==(const PlayerScore&) const = default;
};

/**
 * @brief No player has a life left: the game is over, with each player's
 * score. The server repeats it every 500 ms until it waits for players again.
 */
struct GameEnded {
  static constexpr MessageType TYPE = MessageType::GameEnded;

  std::vector<PlayerScore> players;

  bool operator==(const GameEnded&) const = default;
};

/**
 * @brief Writes the number of players, then each number and score.
 * @throws TooManyPlayersException when there are more than MAXIMUM_PLAYERS.
 * @throws InvalidFieldValueException when a player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS.
 * @throws DuplicatePlayerNumberException when a number appears twice.
 * Nothing is written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const GameEnded& message);

/**
 * @brief Reads a GameEnded from the whole of @p payload. The number of
 * players is checked before any player is read.
 * @throws TooManyPlayersException when it announces more than MAXIMUM_PLAYERS.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException or DuplicatePlayerNumberException for a
 * player number the protocol does not allow.
 * @throws TrailingBytesException when bytes follow the last player.
 */
[[nodiscard]] GameEnded decodePayload(std::span<const std::byte> payload,
                                      std::type_identity<GameEnded> type);

}  // namespace rtype::network
