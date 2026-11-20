#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "LobbyMap.hpp"
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief One player of the waiting room, as LobbyState lists it, with the map
 * it votes for: its number in the maps of the LobbyState, from 1, or
 * NO_MAP_VOTE.
 */
struct LobbyPlayer {
  std::uint8_t playerNumber;
  bool ready;
  std::string nickname;
  std::uint8_t mapVote{NO_MAP_VOTE};

  bool operator==(const LobbyPlayer&) const = default;
};

/**
 * @brief Who is in the waiting room, who is ready, and the maps the players
 * vote for, at most MAXIMUM_LOBBY_MAPS, each with its name and its preview
 * (LobbyMap); none when the server plays one level or the waves. The server
 * sends it on every change, and at least every 500 ms while no game runs.
 */
struct LobbyState {
  static constexpr MessageType TYPE = MessageType::LobbyState;

  std::vector<LobbyPlayer> players;
  std::vector<LobbyMap> maps;

  bool operator==(const LobbyState&) const = default;
};

/**
 * @brief Writes the number of players, then each one: number, ready, nickname,
 * map vote; then the number of maps, then the name and the preview of each.
 * @throws TooManyPlayersException when there are more than MAXIMUM_PLAYERS.
 * @throws InvalidFieldValueException when a player number is not
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS.
 * @throws DuplicatePlayerNumberException when a number appears twice.
 * @throws InvalidNicknameException when a nickname is empty, too long or not
 * UTF-8.
 * @throws InvalidFieldValueException naming mapCount when there are more than
 * MAXIMUM_LOBBY_MAPS maps, mapName for a name that breaks the rules of a
 * nickname, mapPreview for a preview neither empty nor a path isContentPath()
 * accepts, or mapVote for a vote past the last map. Nothing is written when
 * any of them is raised.
 */
void encodePayload(ByteWriter& writer, const LobbyState& message);

/**
 * @brief Reads a LobbyState from the whole of @p payload. The number of
 * players is checked before any player is read, and the number of maps
 * before any map.
 * @throws TooManyPlayersException when it announces more than MAXIMUM_PLAYERS.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidBoolException, InvalidFieldValueException,
 * DuplicatePlayerNumberException or InvalidNicknameException for a player the
 * protocol does not allow, and InvalidFieldValueException for a map the
 * protocol does not allow, as encodePayload() does.
 * @throws TrailingBytesException when bytes follow the last map.
 */
[[nodiscard]] LobbyState decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<LobbyState> type);

}  // namespace rtype::network
