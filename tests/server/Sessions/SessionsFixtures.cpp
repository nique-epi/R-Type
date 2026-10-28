#include "SessionsFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include "ConnectedPlayer.hpp"
#include "MessageOrigin.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

network::MessageOrigin originOf(std::size_t index, std::uint16_t sequence) {
  return network::MessageOrigin{
      .sender = playerEndpoint(index),
      .header = network::PacketHeader{.sequence = sequence}};
}

ConnectedPlayer& addPlayer(PlayerRoster& roster, std::size_t index) {
  return *roster.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                     SESSION_TOKEN);
}

}  // namespace rtype::server::testing
