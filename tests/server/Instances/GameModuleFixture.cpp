#include "GameModuleFixture.hpp"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameContext.hpp"
#include "GameModuleTestConstants.hpp"
#include "IGameModule.hpp"
#include "InstallContext.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "Spawning/WavePlanFixtures.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"

namespace rtype::server::testing {

GameModuleFixture::GameModuleFixture()
    : settings_{.seed = MODULE_TEST_SEED, .waves = testWavePlan()},
      context_{.events = events_,
               .timers = timers_,
               .systems = systems_,
               .world = world_,
               .roster = roster_,
               .settings = settings_,
               .send =
                   [this](network::OutgoingDatagram datagram) {
                     sent_.push_back(std::move(datagram));
                   },
               .everyPlayerIsReady = [] { return false; }} {}

void GameModuleFixture::install(IGameModule& module) {
  engine::InstallContext installation(module.name(), services_, stages_);
  module.install(context_, installation);
}

ConnectedPlayer& GameModuleFixture::join(std::size_t index) {
  return *roster_.add(playerEndpoint(index),
                      std::string{MODULE_PLAYER_NICKNAME}, MODULE_PLAYER_TOKEN);
}

GameContext& GameModuleFixture::context() { return context_; }

void GameModuleFixture::runStages() {
  stages_.run(engine::SIMULATION_TICK_DURATION);
}

game::World& GameModuleFixture::world() { return world_.current(); }

const std::vector<network::OutgoingDatagram>& GameModuleFixture::sentDatagrams()
    const {
  return sent_;
}

}  // namespace rtype::server::testing
