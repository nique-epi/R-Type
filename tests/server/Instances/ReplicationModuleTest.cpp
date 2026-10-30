#include <gtest/gtest.h>
#include "EntityDestruction.hpp"
#include "GameModuleTestConstants.hpp"
#include "IGameReplication.hpp"
#include "MessageType.hpp"
#include "RtypeGameFixture.hpp"

namespace rtype::server::testing {

/**
 * Given a game with no module
 * When the replication module is installed
 * Then the game offers the IGameReplication service to the modules after it
 */
TEST_F(RtypeGameFixture, ReplicationModuleProvidesTheReplicationWhenInstalled) {
  installReplication();

  EXPECT_NE(provided<IGameReplication>(), nullptr);
}

/**
 * Given an installed replication module that was not started, and a player
 * When a destruction is published and the bus delivers it
 * Then nothing is sent, since installing keeps no subscription open
 */
TEST_F(RtypeGameFixture, ReplicationModuleListensToNothingBeforeItStarts) {
  installReplication();
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
TEST_F(RtypeGameFixture, ReplicationModuleBroadcastsTheEventsOnceStarted) {
  installReplication();
  replicationModule().start();
  join(0);

  publishAndDispatch(
      game::EntityDestruction{.networkId = DESTROYED_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});

  expectFirstMessageSentTo(0, network::MessageType::EntityDestroyed);
}

/**
 * Given a started replication module and a player in the roster
 * When the module is stopped, then a destruction is published and delivered
 * Then nothing is sent
 */
TEST_F(RtypeGameFixture, ReplicationModuleStopsListeningOnceStopped) {
  installReplication();
  replicationModule().start();
  join(0);
  replicationModule().stop();

  publishAndDispatch(
      game::EntityDestruction{.networkId = DESTROYED_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});

  EXPECT_TRUE(sentDatagrams().empty());
}

}  // namespace rtype::server::testing
