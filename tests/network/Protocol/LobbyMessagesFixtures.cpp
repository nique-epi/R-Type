#include "LobbyMessagesFixtures.hpp"
#include <cstdint>
#include <string>
#include "LobbyState.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

LobbyPlayer lobbyPlayer(std::uint8_t playerNumber) {
  return LobbyPlayer{.playerNumber = playerNumber,
                     .ready = false,
                     .nickname = std::string{SIXTEEN_BYTE_NICKNAME},
                     .mapVote = NO_MAP_VOTE};
}

LobbyState fullLobby() {
  LobbyState lobby{.players = {lobbyPlayer(1), lobbyPlayer(2), lobbyPlayer(3),
                               lobbyPlayer(4)},
                   .maps = {}};
  for (LobbyPlayer& player : lobby.players) {
    player.mapVote = MAXIMUM_LOBBY_MAPS;
  }
  lobby.maps.assign(MAXIMUM_LOBBY_MAPS, std::string{SIXTEEN_BYTE_NICKNAME});
  return lobby;
}

}  // namespace rtype::network::testing
