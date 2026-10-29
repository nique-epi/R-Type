#include "GamesFixture.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "FarewellSender.hpp"
#include "GameInstance.hpp"
#include "GameInstanceParts.hpp"
#include "GameSettings.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingDatagramSender.hpp"
#include "RtypeGameModules.hpp"
#include "SentDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "Spawning/WavePlanFixtures.hpp"
#include "TimeConstants.hpp"

void GamesFixture::addGame() {
  games_.add(std::make_unique<rtype::server::GameInstance>(
      rtype::server::GameInstanceParts{
          .send = []([[maybe_unused]] const rtype::network::OutgoingDatagram&
                         datagram) {},
          .settings =
              rtype::server::GameSettings{
                  .seed = SOME_SEED,
                  .waves = rtype::server::testing::testWavePlan()},
          .events = events_,
          .timers = timers_,
          .clock = clock_},
      rtype::server::rtypeGameModules()));
}

rtype::server::ConnectedPlayer& GamesFixture::join(std::size_t index) {
  rtype::server::GameInstance& game = *games_.all().front();
  rtype::server::ConnectedPlayer* const player = game.roster().add(
      rtype::server::testing::playerEndpoint(index),
      std::string{PLAYER_NICKNAME}, PLAYER_TOKEN_BASE + index);
  game.onPlayerJoined(*player);
  return *player;
}

rtype::server::Games& GamesFixture::games() { return games_; }

RecordingDatagramSender& GamesFixture::sender() { return sender_; }

rtype::server::FarewellSender& GamesFixture::farewell() { return farewell_; }

const std::vector<rtype::engine::Duration>& GamesFixture::pauses() const {
  return pauses_;
}
