#include "ExtraLifeKeeperFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "EntityTypesTestConstants.hpp"
#include "ExtraLifeKeeperParts.hpp"
#include "ExtraLifeWon.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipSpawnAnnouncerParts.hpp"
#include "ShippedEntitySheets.hpp"

namespace rtype::server::testing {

ExtraLifeKeeperFixture::ExtraLifeKeeperFixture()
    : announcer_(ShipSpawnAnnouncerParts{
          .world = world_,
          .roster = roster_,
          .send =
              [this](network::OutgoingDatagram datagram) {
                sent_.push_back(std::move(datagram));
              },
          .ship = game::testing::shippedEntitySheets().named(
              game::testing::SHIP_SHEET)}),
      scores_(events_, roster_),
      keeper_(ExtraLifeKeeperParts{
          .events = events_, .world = world_, .roster = roster_}) {
  events_.subscribe<game::ExtraLifeWon>(
      [this](const game::ExtraLifeWon& won) { wins_.push_back(won); });
}

ConnectedPlayer& ExtraLifeKeeperFixture::join(std::size_t index) {
  ConnectedPlayer* const player = roster_.add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  announcer_.onPlayerJoined(*player);
  return *player;
}

void ExtraLifeKeeperFixture::kill(const ConnectedPlayer& player,
                                  std::uint32_t points) {
  events_.publish(
      game::EnemyKilled{.playerNumber = player.playerNumber, .points = points});
  events_.dispatch();
}

game::World& ExtraLifeKeeperFixture::world() { return world_; }

const std::vector<game::ExtraLifeWon>& ExtraLifeKeeperFixture::wins() const {
  return wins_;
}

}  // namespace rtype::server::testing
