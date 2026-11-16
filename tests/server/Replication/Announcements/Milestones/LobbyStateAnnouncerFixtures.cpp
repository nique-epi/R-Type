#include "LobbyStateAnnouncerFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include "LobbyState.hpp"
#include "LobbyStateAnnouncerTestConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

network::LobbyPlayer lobbyPlayer(std::size_t index, bool ready) {
  return network::LobbyPlayer{.playerNumber = static_cast<std::uint8_t>(
                                  network::FIRST_PLAYER_NUMBER + index),
                              .ready = ready,
                              .nickname = std::string{PLAYER_NICKNAME}};
}

std::size_t ticksInTheLongestGap() {
  return static_cast<std::size_t>(LONGEST_LOBBY_STATE_GAP /
                                  engine::SIMULATION_TICK_DURATION);
}

}  // namespace rtype::server::testing
