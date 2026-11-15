#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Where a player of the waiting room is: in the room, ready to start or
 * not, or at the roulette table of the casino, where it is never ready.
 */
enum class PilotPlace : std::uint8_t {
  NotReady = 0,
  Ready = 1,
  AtCasino = 2,
};

/**
 * @brief One player of the waiting room, as LobbyState lists it.
 */
struct LobbyPlayer {
  std::uint8_t playerNumber;
  PilotPlace place{PilotPlace::NotReady};
  std::string nickname;

  bool operator==(const LobbyPlayer&) const = default;
};

/**
 * @brief Who is in the waiting room and where each one is. The server sends it
 * on every change, and at least every 500 ms while no game runs.
 */
struct LobbyState {
  static constexpr MessageType TYPE = MessageType::LobbyState;

  std::vector<LobbyPlayer> players;

  bool operator==(const LobbyState&) const = default;
};

/**
 * @brief Writes the number of players, then each one: number, place, nickname.
 * @throws TooManyPlayersException when there are more than MAXIMUM_PLAYERS.
 * @throws InvalidFieldValueException when a player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS, or a place is not one of PilotPlace.
 * @throws DuplicatePlayerNumberException when a number appears twice.
 * @throws InvalidNicknameException when a nickname is empty, too long or not
 * UTF-8. Nothing is written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const LobbyState& message);

/**
 * @brief Reads a LobbyState from the whole of @p payload. The number of
 * players is checked before any player is read.
 * @throws TooManyPlayersException when it announces more than MAXIMUM_PLAYERS.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException,
 * DuplicatePlayerNumberException or InvalidNicknameException for a player the
 * protocol does not allow.
 * @throws TrailingBytesException when bytes follow the last player.
 */
[[nodiscard]] LobbyState decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<LobbyState> type);

}  // namespace rtype::network
