#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
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
 * @brief A game process tells its supervisor where its game stands and how many
 * players are connected to it. Sent every second, under the session token the
 * supervisor gave the game: it holds the whole state, which replaces the one
 * before.
 */
struct GameStatus {
  static constexpr MessageType TYPE = MessageType::GameStatus;

  GameStatusState state{GameStatusState::WaitingForPlayers};
  std::uint8_t playerCount{};

  bool operator==(const GameStatus&) const = default;
};

/**
 * @brief Writes the state, then the number of players.
 * @throws InvalidFieldValueException when the state is not one of
 * GameStatusState, or the number of players is above MAXIMUM_PLAYERS; nothing
 * is written then.
 */
void encodePayload(ByteWriter& writer, const GameStatus& message);

/**
 * @brief Reads a GameStatus from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload holds fewer than two bytes.
 * @throws TrailingBytesException when bytes follow the number of players.
 * @throws InvalidFieldValueException when the state is not one of
 * GameStatusState, or the number of players is above MAXIMUM_PLAYERS.
 */
[[nodiscard]] GameStatus decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<GameStatus> type);

}  // namespace rtype::network
