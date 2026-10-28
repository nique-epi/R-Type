#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "GameModuleFixture.hpp"
#include "GameModules.hpp"
#include "GameModulesFixtures.hpp"
#include "GameModulesTestConstants.hpp"
#include "IGameFlow.hpp"
#include "IGameModule.hpp"
#include "IGameReplication.hpp"
#include "ReplicationModule.hpp"
#include "RtypeRulesModule.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/**
 * Given two modules, the first listed before the second
 * When they are installed and started
 * Then both are installed, in that order, before either is started, and they
 * start in that order
 */
TEST_F(GameModuleFixture, GameModulesInstallEveryModuleThenStartThemInOrder) {
  std::vector<std::string> journal;
  GameModules modules(twoRecordingModules(journal, false));

  modules.installAndStart(context());

  EXPECT_EQ(journal, journalOf({FIRST_INSTALLED, SECOND_INSTALLED,
                                FIRST_STARTED, SECOND_STARTED}));
}

/**
 * Given two modules that were installed and started
 * When the modules are destroyed
 * Then the second is stopped, then the first
 */
TEST_F(GameModuleFixture, GameModulesStopTheModulesInTheOppositeOrder) {
  std::vector<std::string> journal;
  {
    GameModules modules(twoRecordingModules(journal, false));
    modules.installAndStart(context());
    journal.clear();
  }

  EXPECT_EQ(journal, journalOf({SECOND_STOPPED, FIRST_STOPPED}));
}

/**
 * Given two modules, the start of the second of which throws
 * When they are installed and started, then destroyed
 * Then the first is stopped and the second, which never started, is not
 */
TEST_F(GameModuleFixture, GameModulesStopOnlyTheModulesThatStarted) {
  std::vector<std::string> journal;
  {
    GameModules modules(twoRecordingModules(journal, true));
    EXPECT_THROW(modules.installAndStart(context()), engine::EngineException);
  }

  EXPECT_EQ(journal, journalOf({FIRST_INSTALLED, SECOND_INSTALLED,
                                FIRST_STARTED, SECOND_STARTED, FIRST_STOPPED}));
}

/**
 * Given two started modules that each added work to the three stages
 * When the stages run
 * Then every work of the input stage runs, then every work of the simulation
 * stage, then every work of the output stage, each in the order of the modules
 */
TEST_F(GameModuleFixture, GameModulesRunTheStagesInOrder) {
  std::vector<std::string> journal;
  GameModules modules(twoRecordingModules(journal, false));
  modules.installAndStart(context());
  journal.clear();

  modules.runStages(engine::SIMULATION_TICK_DURATION);

  EXPECT_EQ(journal,
            journalOf({FIRST_IN_INPUT_STAGE, SECOND_IN_INPUT_STAGE,
                       FIRST_IN_SIMULATION_STAGE, SECOND_IN_SIMULATION_STAGE,
                       FIRST_IN_OUTPUT_STAGE, SECOND_IN_OUTPUT_STAGE}));
}

/**
 * Given the rules of R-Type listed with no replication before them
 * When the modules are installed
 * Then the installation fails on the missing service
 */
TEST_F(GameModuleFixture, GameModulesRefuseAModuleWhoseServiceIsMissing) {
  std::vector<std::unique_ptr<IGameModule>> listed;
  listed.push_back(std::make_unique<RtypeRulesModule>());
  GameModules modules(std::move(listed));

  EXPECT_THROW(modules.installAndStart(context()),
               engine::MissingServiceException);
}

/**
 * Given the replication module, installed and started
 * When the replication service is required
 * Then it is found, and the flow service, which no module provides, is not
 */
TEST_F(GameModuleFixture, GameModulesGiveTheServicesTheirModulesProvided) {
  std::vector<std::unique_ptr<IGameModule>> listed;
  listed.push_back(std::make_unique<ReplicationModule>());
  GameModules modules(std::move(listed));
  modules.installAndStart(context());

  EXPECT_NO_THROW(static_cast<void>(modules.require<IGameReplication>()));
  EXPECT_THROW(static_cast<void>(modules.require<IGameFlow>()),
               engine::MissingServiceException);
}

/**
 * Given two started modules and a player in the roster
 * When the functions added to the hooks for a joined player are called
 * Then both modules are told, in the order of the modules
 */
TEST_F(GameModuleFixture, GameModulesTellEveryModuleOfAPlayerWhoJoined) {
  std::vector<std::string> journal;
  GameModules modules(twoRecordingModules(journal, false));
  modules.installAndStart(context());
  ConnectedPlayer& player = join(0);
  journal.clear();

  playerJoined(player);

  EXPECT_EQ(journal, journalOf({FIRST_TOLD_OF_A_JOIN, SECOND_TOLD_OF_A_JOIN}));
}

/**
 * Given two started modules and a player in the roster
 * When the functions added to the hooks for a closing session are called
 * Then both modules are told, in the order of the modules
 */
TEST_F(GameModuleFixture, GameModulesTellEveryModuleOfAClosingSession) {
  std::vector<std::string> journal;
  GameModules modules(twoRecordingModules(journal, false));
  modules.installAndStart(context());
  const ConnectedPlayer& player = join(0);
  journal.clear();

  sessionClosing(player);

  EXPECT_EQ(journal,
            journalOf({FIRST_TOLD_OF_A_DEPARTURE, SECOND_TOLD_OF_A_DEPARTURE}));
}

}  // namespace rtype::server::testing
