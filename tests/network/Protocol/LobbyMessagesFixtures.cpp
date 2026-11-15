#include "LobbyMessagesFixtures.hpp"
#include <cstdint>
#include <string>
#include "LobbyState.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

LobbyPlayer lobbyPlayer(std::uint8_t playerNumber) {
  return LobbyPlayer{.playerNumber = playerNumber,
                     .place = PilotPlace::NotReady,
                     .nickname = std::string{SIXTEEN_BYTE_NICKNAME}};
}

LobbyState fullLobby() {
  LobbyState lobby;
  for (std::uint8_t number = FIRST_PLAYER_NUMBER; number <= MAXIMUM_PLAYERS;
       ++number) {
    lobby.players.push_back(lobbyPlayer(number));
  }
  return lobby;
}

}  // namespace rtype::network::testing
