#include "RunningGameFixture.hpp"
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
#include "PlayerRoster.hpp"
#include "RecordingDatagramSender.hpp"
#include "RtypeGameModules.hpp"
#include "RunningGame.hpp"
#include "SentDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "Spawning/WavePlanFixtures.hpp"
#include "TimeConstants.hpp"

void RunningGameFixture::hostGame() {
  runningGame_.host(std::make_unique<rtype::server::GameInstance>(
      rtype::server::GameInstanceParts{
          .send = []([[maybe_unused]] const rtype::network::OutgoingDatagram&
                         datagram) {},
          .settings =
              rtype::server::GameSettings{
                  .seed = SOME_SEED,
                  .waves = rtype::server::testing::testWavePlan(),
                  .types = rtype::game::testing::shippedEntitySheets()},
          .events = runningGame_.events(),
          .clock = clock_},
      rtype::server::rtypeGameModules(results_)));
}

rtype::server::ConnectedPlayer& RunningGameFixture::join(std::size_t index) {
  rtype::server::GameInstance& game = runningGame_.game();
  rtype::server::ConnectedPlayer* const player = game.roster().add(
      rtype::server::testing::playerEndpoint(index),
      std::string{PLAYER_NICKNAME}, PLAYER_TOKEN_BASE + index);
  game.onPlayerJoined(*player);
  return *player;
}

rtype::server::RunningGame& RunningGameFixture::runningGame() {
  return runningGame_;
}

rtype::server::PlayerRoster& RunningGameFixture::roster() {
  return runningGame_.game().roster();
}

RecordingDatagramSender& RunningGameFixture::sender() { return sender_; }

rtype::server::FarewellSender& RunningGameFixture::farewell() {
  return farewell_;
}

const std::vector<rtype::engine::Duration>& RunningGameFixture::pauses() const {
  return pauses_;
}
