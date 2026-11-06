#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "GameStatusMeasures.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Where a game stands, as a game process tells its supervisor. The
 * values are the ones of the protocol page; any other byte is invalid.
 */
enum class GameStatusState : std::uint8_t {
  WaitingForPlayers = 1,
  InProgress = 2,
  Finished = 3,
};

/**
 * @brief A game process tells its supervisor where its game stands, who plays
 * it and what the process measured. Sent every second, under the session token
 * the supervisor gave the game: it holds the whole state, which replaces the
 * one before.
 */
struct GameStatus {
  static constexpr MessageType TYPE = MessageType::GameStatus;

  GameStatusState state{GameStatusState::WaitingForPlayers};
  /** @brief The nickname of each player connected to the game, in the order
   * of their numbers: MAXIMUM_PLAYERS at most. */
  std::vector<std::string> nicknames;
  GameStatusMeasures measures;

  bool operator==(const GameStatus&) const = default;
};

/**
 * @brief Writes the state, the number of nicknames, each nickname, the
 * measures byte (one bit per measure taken, see
 * TICK_MEDIAN_MEASURE_BIT_POSITION), then each measure taken as a 32-bit
 * count of microseconds.
 * @throws InvalidFieldValueException when the state is not one of
 * GameStatusState.
 * @throws TooManyPlayersException when there are more than MAXIMUM_PLAYERS
 * nicknames.
 * @throws InvalidNicknameException when a nickname is not one the protocol
 * accepts. Nothing is written then.
 */
void encodePayload(ByteWriter& writer, const GameStatus& message);

/**
 * @brief Reads a GameStatus from the whole of @p payload. The number of
 * nicknames is checked before any nickname is read.
 * @throws BufferUnderflowException when @p payload ends before the last field
 * it announces.
 * @throws TrailingBytesException when bytes follow the last measure.
 * @throws TooManyPlayersException when it announces more than MAXIMUM_PLAYERS
 * nicknames.
 * @throws InvalidFieldValueException when the state is not one of
 * GameStatusState, or the measures byte sets a bit no measure uses.
 * @throws InvalidNicknameException when a nickname is not one the protocol
 * accepts.
 */
[[nodiscard]] GameStatus decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<GameStatus> type);

}  // namespace rtype::network
