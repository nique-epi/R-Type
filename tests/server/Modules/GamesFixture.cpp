#include "GamesFixture.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameInstance.hpp"
#include "GameSettings.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "Spawning/WavePlanFixtures.hpp"

void GamesFixture::addGame() {
  games_.add(std::make_unique<rtype::server::GameInstance>(
      []([[maybe_unused]] rtype::network::OutgoingDatagram datagram) {},
      rtype::server::GameSettings{
          .seed = SOME_SEED, .waves = rtype::server::testing::testWavePlan()},
      events_));
}

rtype::server::ConnectedPlayer& GamesFixture::join(std::size_t gameIndex,
                                                   std::size_t index) {
  rtype::server::GameInstance& game = *games_.all()[gameIndex];
  rtype::server::ConnectedPlayer* const player = game.roster().add(
      rtype::server::testing::playerEndpoint(index),
      std::string{PLAYER_NICKNAME}, PLAYER_TOKEN_BASE + index);
  game.onPlayerJoined(*player);
  return *player;
}
