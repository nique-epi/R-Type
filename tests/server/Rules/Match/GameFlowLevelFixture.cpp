#include "GameFlowLevelFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourScripts.hpp"
#include "ConnectedPlayer.hpp"
#include "EnemyCount.hpp"
#include "EntityIdConstants.hpp"
#include "GameFlowFixture.hpp"
#include "GameFlowLevelTestConstants.hpp"
#include "GameSettings.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "PlayerLives.hpp"
#include "ScriptSource.hpp"
#include "ShippedEntitySheets.hpp"
#include "Spawning/WavePlanFixtures.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The scripts that registered LEVELS_OF_THE_FLOW_TESTS, read once
 * and kept for every test, since they must outlive each game. */
game::BehaviourScripts& scriptsOfTheFlowLevels() {
  static game::BehaviourScripts scripts{[](const std::string&) {}};
  static const bool read = [] {
    const std::vector<engine::ScriptSource> sources{
        engine::ScriptSource{.name = std::string{FLOW_LEVELS_SCRIPT_NAME},
                             .text = std::string{LEVELS_OF_THE_FLOW_TESTS}}};
    scripts.run(sources);
    return true;
  }();
  static_cast<void>(read);
  return scripts;
}

}  // namespace

GameFlowLevelFixture::GameFlowLevelFixture(std::string_view level)
    : GameFlowFixture(
          GameSettings{.seed = SOME_SEED,
                       .waves = testWavePlan(),
                       .types = game::testing::shippedEntitySheets(),
                       .level = std::string{level},
                       .scripts = &scriptsOfTheFlowLevels()}) {}

void GameFlowLevelFixture::destroyTheLastShipOf(ConnectedPlayer& player) {
  ASSERT_TRUE(flow().world().removePlayer(player.shipEntityId));
  player.shipEntityId = identifiers::NO_ENTITY_ID;
  player.livesWithoutShip = 0;
}

std::uint8_t GameFlowLevelFixture::livesLeftTo(const ConnectedPlayer& player) {
  return livesOf(flow().world(), player);
}

std::size_t GameFlowLevelFixture::enemiesInPlay() {
  return game::enemiesIn(flow().world().components());
}

GameFlowTwoStagesFixture::GameFlowTwoStagesFixture()
    : GameFlowLevelFixture(LEVEL_OF_TWO_STAGES_NAME) {}

GameFlowOneStageFixture::GameFlowOneStageFixture()
    : GameFlowLevelFixture(LEVEL_OF_ONE_STAGE_NAME) {}

GameFlowBrokenLevelFixture::GameFlowBrokenLevelFixture()
    : GameFlowLevelFixture(BROKEN_LEVEL_NAME) {}

}  // namespace rtype::server::testing
