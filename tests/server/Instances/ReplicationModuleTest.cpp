#include <gtest/gtest.h>
#include "EntityDestruction.hpp"
#include "GameModuleFixture.hpp"
#include "GameModuleTestConstants.hpp"
#include "IGameReplication.hpp"
#include "MessageType.hpp"
#include "ReplicationModule.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

/**
 * Given a game with no module
 * When the replication module is installed
 * Then the game offers the IGameReplication service to the modules after it
 */
TEST_F(GameModuleFixture,
       ReplicationModuleProvidesTheReplicationWhenInstalled) {
  ReplicationModule module;

  install(module);

  EXPECT_NE(provided<IGameReplication>(), nullptr);
}

/**
 * Given an installed replication module that was not started, and a player
 * When a destruction is published and the bus delivers it
 * Then nothing is sent, since installing keeps no subscription open
 */
TEST_F(GameModuleFixture, ReplicationModuleListensToNothingBeforeItStarts) {
  ReplicationModule module;
  install(module);
  join(0);

  publishAndDispatch(
      game::EntityDestruction{.networkId = DESTROYED_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given a started replication module and a player in the roster
 * When a destruction is published and the bus delivers it
 * Then the player is sent an EntityDestroyed
 */
TEST_F(GameModuleFixture, ReplicationModuleBroadcastsTheEventsOnceStarted) {
  ReplicationModule module;
  install(module);
  module.start();
  join(0);

  publishAndDispatch(
      game::EntityDestruction{.networkId = DESTROYED_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});

  const auto received = sentTo(sentDatagrams(), playerEndpoint(0));
  ASSERT_FALSE(received.empty());
  EXPECT_EQ(onlyMessageTypeOf(received.front()),
            network::MessageType::EntityDestroyed);
}

/**
 * Given a started replication module and a player in the roster
 * When the module is stopped, then a destruction is published and delivered
 * Then nothing is sent
 */
TEST_F(GameModuleFixture, ReplicationModuleStopsListeningOnceStopped) {
  ReplicationModule module;
  install(module);
  module.start();
  join(0);
  module.stop();

  publishAndDispatch(
      game::EntityDestruction{.networkId = DESTROYED_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});

  EXPECT_TRUE(sentDatagrams().empty());
}

}  // namespace rtype::server::testing
