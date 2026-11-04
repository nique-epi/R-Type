#include "LobbyStateAnnouncerFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

ConnectedPlayer& LobbyStateAnnouncerFixture::join(std::size_t index,
                                                  std::uint64_t sessionToken) {
  ConnectedPlayer* const player = roster_.add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, sessionToken);
  return *player;
}

void LobbyStateAnnouncerFixture::runWaitingTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    announcer_.onWaitingTick();
  }
}

std::vector<network::OutgoingDatagram> LobbyStateAnnouncerFixture::sentTo(
    std::size_t index) const {
  return testing::sentTo(sent_, playerEndpoint(index));
}

const std::vector<network::OutgoingDatagram>& LobbyStateAnnouncerFixture::sent()
    const {
  return sent_;
}

}  // namespace rtype::server::testing
