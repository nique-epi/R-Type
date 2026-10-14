#include <gtest/gtest.h>
#include <vector>
#include "DatagramCodec.hpp"
#include "EntityDamage.hpp"
#include "EntityDestroyed.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "GameEvent.hpp"
#include "GameEventBroadcasterFixture.hpp"
#include "GameEventConstants.hpp"
#include "GameEventTestConstants.hpp"
#include "NetworkException.hpp"
#include "PlayerHit.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

/**
 * Given two clients
 * When a monster spawn is published and a second goes by
 * Then each client gets one EntitySpawned with the monster's id, type and
 * position, and no copy follows
 */
TEST_F(GameEventBroadcasterFixture, SendsASpawnOnceToEachClient) {
  publishAndDispatch(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                       .type = game::EntityType::Enemy,
                                       .x = MONSTER_X,
                                       .y = MONSTER_Y});
  advanceTimers(ONE_SECOND);

  const std::vector<network::EntitySpawned> expected{
      {.entityId = MONSTER_NETWORK_ID,
       .entityType = network::WireEntityType::Bydo,
       .x = MONSTER_X,
       .y = MONSTER_Y}};
  EXPECT_EQ(receivedBy<network::EntitySpawned>(firstClient()), expected);
  EXPECT_EQ(receivedBy<network::EntitySpawned>(secondClient()), expected);
}

/**
 * Given two clients
 * When a destruction is published and time advances
 * Then each client gets the first copy at once, the second at one interval,
 * the third at two, and none after, all identical
 */
TEST_F(GameEventBroadcasterFixture, RepeatsADestructionThreeTimes) {
  publishAndDispatch(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});
  EXPECT_EQ(receivedBy<network::EntityDestroyed>(firstClient()).size(), 1U);

  advanceTimers(JUST_BEFORE_ONE_INTERVAL);
  EXPECT_EQ(receivedBy<network::EntityDestroyed>(firstClient()).size(), 1U);

  advanceTimers(ONE_INTERVAL - JUST_BEFORE_ONE_INTERVAL);
  EXPECT_EQ(receivedBy<network::EntityDestroyed>(firstClient()).size(), 2U);

  advanceTimers(ONE_INTERVAL);
  advanceTimers(ONE_SECOND);
  const std::vector<network::EntityDestroyed> expectedCopies(
      REPEATED_MESSAGE_COPIES,
      network::EntityDestroyed{.entityId = MONSTER_NETWORK_ID,
                               .reason = network::DestructionReason::Killed});
  EXPECT_EQ(receivedBy<network::EntityDestroyed>(firstClient()),
            expectedCopies);
  EXPECT_EQ(receivedBy<network::EntityDestroyed>(secondClient()),
            expectedCopies);
}

/**
 * Given two clients
 * When a player hit is published and a second goes by
 * Then each client gets three GameEvents, all with the same number
 */
TEST_F(GameEventBroadcasterFixture, RepeatsAGameEventWithTheSameNumber) {
  publishAndDispatch(game::PlayerHit{.playerNumber = SECOND_PLAYER_NUMBER});
  advanceTimers(ONE_SECOND);

  const std::vector<network::GameEvent> expected(
      REPEATED_MESSAGE_COPIES,
      network::GameEvent{.eventNumber = FIRST_EVENT_NUMBER,
                         .kind = network::GameEventKind::PlayerHit,
                         .playerNumber = SECOND_PLAYER_NUMBER,
                         .entityId = 0});
  EXPECT_EQ(receivedBy<network::GameEvent>(firstClient()), expected);
  EXPECT_EQ(receivedBy<network::GameEvent>(secondClient()), expected);
}

/**
 * Given one client when a destruction is published, and a second client that
 * joins right after
 * When the later copies come due
 * Then the newcomer gets none of them, and the first client gets all three
 */
TEST_F(GameEventBroadcasterFixture, SendsTheCopiesOnlyToTheClientsOfTheEvent) {
  setRecipients({firstClient()});
  publishAndDispatch(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});

  setRecipients({firstClient(), secondClient()});
  advanceTimers(ONE_SECOND);

  EXPECT_EQ(receivedBy<network::EntityDestroyed>(firstClient()).size(),
            REPEATED_MESSAGE_COPIES);
  EXPECT_TRUE(receivedBy<network::EntityDestroyed>(secondClient()).empty());
}

/**
 * Given no client
 * When events are published and a second goes by
 * Then nothing is sent
 */
TEST_F(GameEventBroadcasterFixture, SendsNothingWithoutAClient) {
  setRecipients({});
  publishAndDispatch(game::PlayerHit{.playerNumber = SECOND_PLAYER_NUMBER});
  advanceTimers(ONE_SECOND);

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given two clients
 * When a spawn is published
 * Then each of the two datagrams carries the header asked for it, so they do
 * not share a sequence number
 */
TEST_F(GameEventBroadcasterFixture, GivesEachDatagramItsOwnSequenceNumber) {
  publishAndDispatch(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                       .type = game::EntityType::Enemy,
                                       .x = MONSTER_X,
                                       .y = MONSTER_Y});

  ASSERT_EQ(sentDatagrams().size(), 2U);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(sentDatagrams().at(0).payload)
                .sequence,
            1U);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(sentDatagrams().at(1).payload)
                .sequence,
            2U);
}

/**
 * Given two clients
 * When an event naming the entity id 0 is published
 * Then dispatching throws and nothing is sent
 */
TEST_F(GameEventBroadcasterFixture, SendsNothingForAnEventWithoutAnEntityId) {
  publish(game::EntityDamage{.networkId = 0});

  EXPECT_THROW(dispatch(), network::InvalidFieldValueException);
  EXPECT_TRUE(sentDatagrams().empty());
}

}  // namespace rtype::server::testing
