#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
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
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "PlayerLeft.hpp"
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
  advanceTimers(ALL_COPIES_SENT);

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
  advanceTimers(ALL_COPIES_SENT);
  const std::vector<network::EntityDestroyed> expectedCopies(
      PROTOCOL_REPEATED_MESSAGE_COPIES,
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
  advanceTimers(ALL_COPIES_SENT);

  const std::vector<network::GameEvent> expected(
      PROTOCOL_REPEATED_MESSAGE_COPIES,
      network::GameEvent{.eventNumber = FIRST_EVENT_NUMBER,
                         .kind = network::GameEventKind::PlayerHit,
                         .playerNumber = SECOND_PLAYER_NUMBER,
                         .entityId = 0});
  EXPECT_EQ(receivedBy<network::GameEvent>(firstClient()), expected);
  EXPECT_EQ(receivedBy<network::GameEvent>(secondClient()), expected);
}

/**
 * Given two clients
 * When a destruction is published and every copy comes due
 * Then, for each client, the sequence number of each copy is greater than the
 * one of the copy before
 */
TEST_F(GameEventBroadcasterFixture, GivesEachCopyANewerSequenceNumber) {
  publishAndDispatch(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});
  advanceTimers(ALL_COPIES_SENT);

  const std::vector<network::Endpoint> clients{firstClient(), secondClient()};
  for (const network::Endpoint& client : clients) {
    std::vector<std::uint16_t> sequences;
    for (const network::OutgoingDatagram& datagram : sentDatagrams()) {
      if (datagram.destination == client) {
        sequences.push_back(
            network::DatagramCodec::decodeHeader(datagram.payload).sequence);
      }
    }
    ASSERT_EQ(sequences.size(), PROTOCOL_REPEATED_MESSAGE_COPIES);
    for (std::size_t copy = 1; copy < sequences.size(); ++copy) {
      EXPECT_GT(sequences.at(copy), sequences.at(copy - 1));
    }
  }
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
  advanceTimers(ALL_COPIES_SENT);

  EXPECT_EQ(receivedBy<network::EntityDestroyed>(firstClient()).size(),
            PROTOCOL_REPEATED_MESSAGE_COPIES);
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
  advanceTimers(ALL_COPIES_SENT);

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given two clients
 * When a spawn is published
 * Then each of the two datagrams carries the header asked for its own
 * recipient, and the two do not share a sequence number
 */
TEST_F(GameEventBroadcasterFixture, AsksForTheHeaderOfEachDatagramRecipient) {
  publishAndDispatch(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                       .type = game::EntityType::Enemy,
                                       .x = MONSTER_X,
                                       .y = MONSTER_Y});

  ASSERT_EQ(sentDatagrams().size(), 2U);
  const network::PacketHeader first =
      network::DatagramCodec::decodeHeader(sentDatagrams().at(0).payload);
  const network::PacketHeader second =
      network::DatagramCodec::decodeHeader(sentDatagrams().at(1).payload);
  EXPECT_EQ(first.sessionToken, FIRST_CLIENT_PORT);
  EXPECT_EQ(second.sessionToken, SECOND_CLIENT_PORT);
  EXPECT_EQ(first.sequence, 1U);
  EXPECT_EQ(second.sequence, 2U);
}

/**
 * Given two clients
 * When a spawn whose position is not a number is published, then a valid spawn
 * Then dispatching does not throw, the first spawn sends nothing, and the
 * second reaches both clients
 */
TEST_F(GameEventBroadcasterFixture, SkipsASpawnWhoseCoordinateIsNotANumber) {
  EXPECT_NO_THROW(
      publishAndDispatch(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                           .type = game::EntityType::Enemy,
                                           .x = NOT_A_NUMBER,
                                           .y = MONSTER_Y}));
  advanceTimers(ALL_COPIES_SENT);
  EXPECT_TRUE(sentDatagrams().empty());

  publishAndDispatch(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                       .type = game::EntityType::Enemy,
                                       .x = MONSTER_X,
                                       .y = MONSTER_Y});

  EXPECT_EQ(receivedBy<network::EntitySpawned>(firstClient()).size(), 1U);
  EXPECT_EQ(receivedBy<network::EntitySpawned>(secondClient()).size(), 1U);
}

/**
 * Given two clients
 * When an entity damage naming the network id 0 is published, then a player
 * hit
 * Then dispatching does not throw, no copy of the damage is ever sent, and the
 * player hit carries the first event number
 */
TEST_F(GameEventBroadcasterFixture,
       SkipsAnEventWithoutAnEntityIdAndKeepsItsNumber) {
  EXPECT_NO_THROW(publishAndDispatch(
      game::EntityDamage{.networkId = UNASSIGNED_NETWORK_ID}));
  advanceTimers(ALL_COPIES_SENT);
  EXPECT_TRUE(sentDatagrams().empty());

  publishAndDispatch(game::PlayerHit{.playerNumber = SECOND_PLAYER_NUMBER});

  const std::vector<network::GameEvent> received =
      receivedBy<network::GameEvent>(firstClient());
  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(received.front().eventNumber, FIRST_EVENT_NUMBER);
  EXPECT_EQ(received.front().kind, network::GameEventKind::PlayerHit);
}

/**
 * Given two clients
 * When a player hit for a player outside the game is published, then a player
 * elimination for a player of the game
 * Then dispatching does not throw, the hit sends nothing, and the elimination
 * reaches both clients
 */
TEST_F(GameEventBroadcasterFixture, SkipsAPlayerEventOfAPlayerOutsideTheGame) {
  EXPECT_NO_THROW(publishAndDispatch(
      game::PlayerHit{.playerNumber = PLAYER_NUMBER_OUTSIDE_THE_GAME}));
  advanceTimers(ALL_COPIES_SENT);
  EXPECT_TRUE(sentDatagrams().empty());

  publishAndDispatch(
      game::PlayerElimination{.playerNumber = SECOND_PLAYER_NUMBER});
  advanceTimers(ALL_COPIES_SENT);

  EXPECT_EQ(receivedBy<network::GameEvent>(firstClient()).size(),
            PROTOCOL_REPEATED_MESSAGE_COPIES);
  EXPECT_EQ(receivedBy<network::GameEvent>(secondClient()).size(),
            PROTOCOL_REPEATED_MESSAGE_COPIES);
}

/**
 * Given two clients
 * When a player that timed out is published and every copy comes due
 * Then each client gets three identical PlayerLeft naming that player
 */
TEST_F(GameEventBroadcasterFixture, RepeatsADepartureThreeTimes) {
  publishAndDispatch(
      game::PlayerDeparture{.playerNumber = SECOND_PLAYER_NUMBER,
                            .cause = game::DepartureCause::TimedOut});
  advanceTimers(ALL_COPIES_SENT);

  const std::vector<network::PlayerLeft> expected(
      PROTOCOL_REPEATED_MESSAGE_COPIES,
      network::PlayerLeft{.playerNumber = SECOND_PLAYER_NUMBER,
                          .reason = network::DepartureReason::TimedOut});
  EXPECT_EQ(receivedBy<network::PlayerLeft>(firstClient()), expected);
  EXPECT_EQ(receivedBy<network::PlayerLeft>(secondClient()), expected);
}

/**
 * Given two clients
 * When the departure of a player outside the game is published
 * Then dispatching does not throw and nothing is sent
 */
TEST_F(GameEventBroadcasterFixture, SkipsTheDepartureOfAPlayerOutsideTheGame) {
  EXPECT_NO_THROW(publishAndDispatch(
      game::PlayerDeparture{.playerNumber = PLAYER_NUMBER_OUTSIDE_THE_GAME,
                            .cause = game::DepartureCause::TimedOut}));
  advanceTimers(ALL_COPIES_SENT);

  EXPECT_TRUE(sentDatagrams().empty());
}

}  // namespace rtype::server::testing
