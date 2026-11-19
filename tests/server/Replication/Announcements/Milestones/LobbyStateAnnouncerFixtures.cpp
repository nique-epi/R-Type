#include "LobbyStateAnnouncerFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
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
                              .nickname = std::string{PLAYER_NICKNAME},
                              .mapVote = network::NO_MAP_VOTE};
}

network::LobbyState announcedRoom(std::vector<network::LobbyPlayer> players) {
  return network::LobbyState{.players = std::move(players),
                             .maps = {std::string{FIRST_ANNOUNCED_MAP},
                                      std::string{SECOND_ANNOUNCED_MAP}}};
}

std::size_t ticksInTheLongestGap() {
  return static_cast<std::size_t>(LONGEST_LOBBY_STATE_GAP /
                                  engine::SIMULATION_TICK_DURATION);
}

}  // namespace rtype::server::testing
