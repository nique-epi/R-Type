#include "ShipSpawnAnnouncerFixture.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "World.hpp"

namespace rtype::server::testing {

ConnectedPlayer& ShipSpawnAnnouncerFixture::join(std::size_t index) {
  ConnectedPlayer* const player = roster_.add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  announcer_.onPlayerJoined(*player);
  return *player;
}

std::vector<network::OutgoingDatagram> ShipSpawnAnnouncerFixture::sentTo(
    std::size_t index) const {
  return testing::sentTo(sent_, playerEndpoint(index));
}

game::World& ShipSpawnAnnouncerFixture::world() { return world_; }

const std::vector<network::OutgoingDatagram>& ShipSpawnAnnouncerFixture::sent()
    const {
  return sent_;
}

}  // namespace rtype::server::testing
