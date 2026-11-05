#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "DeltaWorldState.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityDestroyed.hpp"
#include "EntityMessage.hpp"
#include "EntityMessageQueueFixture.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "EntitySpawned.hpp"
#include "GameEvent.hpp"
#include "GameStarted.hpp"
#include "PlayerLeft.hpp"
#include "ProtocolConstants.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFilterTestConstants.hpp"

namespace rtype::client {

using testing::APPLIED_TICK;
using testing::damaged;
using ::testing::ElementsAre;
using testing::endOfOnePilot;
using testing::EntityMessageQueueFixture;
using testing::EVENT_NUMBER;
using testing::FIRST_POSITION;
using testing::FIRST_SEQUENCE;
using testing::FOURTH_SEQUENCE;
using ::testing::IsEmpty;
using testing::killed;
using testing::listed;
using testing::OTHER_PLAYER_NUMBER;
using testing::PROTOCOL_SILENCE_LIMIT;
using testing::SECOND_POSITION;
using testing::SECOND_SEQUENCE;
using testing::SHIP_ID;
using ::testing::SizeIs;
using testing::spawned;
using testing::stateListing;
using testing::THIRD_SEQUENCE;

/**
 * Given a game started in the session
 * When an EntitySpawned, a WorldState and an EntityDestroyed arrive, in that
 * order
 * Then the connection hands over the three, in the order they arrived
 */
TEST_F(EntityMessageQueueFixture, HandsOverTheEntityMessagesOfTheGameInOrder) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  const network::EntitySpawned spawn =
      spawned(SHIP_ID, network::WireEntityType::PlayerShip, FIRST_POSITION);
  const network::WorldState state = stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, network::WireEntityType::PlayerShip, SECOND_POSITION)});
  const network::EntityDestroyed kill = killed(SHIP_ID);
  deliverInSession(spawn, SECOND_SEQUENCE);
  deliverWorldStateInSession(state, THIRD_SEQUENCE);
  deliverInSession(kill, FOURTH_SEQUENCE);

  update();

  EXPECT_THAT(drainedEntityMessages(),
              ElementsAre(EntityMessage{spawn}, EntityMessage{state},
                          EntityMessage{kill}));
}

/**
 * Given a session whose game has not started
 * When an EntitySpawned arrives, as for the ship of a player who joins
 * Then the connection keeps nothing: the first WorldState of the game will
 * list that ship
 */
TEST_F(EntityMessageQueueFixture, KeepsNoEntityMessageBeforeTheGameStarts) {
  accept();
  deliverInSession(
      spawned(SHIP_ID, network::WireEntityType::PlayerShip, FIRST_POSITION),
      FIRST_SEQUENCE);

  update();

  EXPECT_THAT(drainedEntityMessages(), IsEmpty());
}

/**
 * Given a session whose GameStarted was lost
 * When the first WorldState arrives
 * Then the connection hands it over: the state that starts the game is the
 * first of the game
 */
TEST_F(EntityMessageQueueFixture, HandsOverTheWorldStateThatStartsTheGame) {
  accept();
  const network::WorldState state = stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, network::WireEntityType::PlayerShip, FIRST_POSITION)});
  deliverWorldStateInSession(state, FIRST_SEQUENCE);

  update();

  EXPECT_THAT(drainedEntityMessages(), ElementsAre(EntityMessage{state}));
}

/**
 * Given a session whose GameStarted was lost
 * When the first state arrives as a DeltaWorldState written without reference
 * Then the connection hands over the state rebuilt in full, as it hands over a
 * WorldState
 */
TEST_F(EntityMessageQueueFixture, HandsOverTheStateRebuiltFromADelta) {
  accept();
  const network::EntityState ship =
      listed(SHIP_ID, network::WireEntityType::PlayerShip, FIRST_POSITION);
  deliverDeltaInSession(
      network::DeltaWorldState{.tick = APPLIED_TICK,
                               .baseDistance = network::NO_REFERENCE_DISTANCE,
                               .partIndex = 0,
                               .partCount = 1,
                               .players = {},
                               .changed = {ship},
                               .removed = {}},
      FIRST_SEQUENCE);

  update();

  EXPECT_THAT(drainedEntityMessages(),
              ElementsAre(EntityMessage{stateListing(APPLIED_TICK, {ship})}));
}

/**
 * Given a game started, and an EntitySpawned already handed over
 * When the connection is drained again
 * Then it hands over nothing
 */
TEST_F(EntityMessageQueueFixture, HandsOverEachEntityMessageOnce) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  deliverInSession(
      spawned(SHIP_ID, network::WireEntityType::PlayerShip, FIRST_POSITION),
      SECOND_SEQUENCE);
  update();
  ASSERT_THAT(drainedEntityMessages(), SizeIs(1));

  EXPECT_THAT(drainedEntityMessages(), IsEmpty());
}

/**
 * Given a game started, and an EntitySpawned not handed over yet
 * When the player leaves the session
 * Then the connection hands over nothing
 */
TEST_F(EntityMessageQueueFixture, LeavingDropsTheEntityMessagesNotHandedOver) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  deliverInSession(
      spawned(SHIP_ID, network::WireEntityType::PlayerShip, FIRST_POSITION),
      SECOND_SEQUENCE);
  update();

  session().cancel();

  EXPECT_THAT(drainedEntityMessages(), IsEmpty());
}

/**
 * Given a session whose game has not started
 * When a GameEvent arrives
 * Then the connection hands over nothing
 */
TEST_F(EntityMessageQueueFixture, IgnoresAGameEventBeforeTheGame) {
  accept();
  deliverInSession(damaged(EVENT_NUMBER, SHIP_ID), FIRST_SEQUENCE);

  update();

  EXPECT_THAT(drainedEntityMessages(), IsEmpty());
}

/**
 * Given a session that handed over an event, then was left, and a new
 * session whose game started
 * When an event of the same number arrives
 * Then it is handed over: a new session numbers its events again
 */
TEST_F(EntityMessageQueueFixture, ANewSessionForgetsTheEventsSeen) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  const network::GameEvent event = damaged(EVENT_NUMBER, SHIP_ID);
  deliverInSession(event, SECOND_SEQUENCE);
  update();
  ASSERT_THAT(drainedEntityMessages(), SizeIs(1));
  session().cancel();
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  deliverInSession(event, SECOND_SEQUENCE);

  update();

  EXPECT_THAT(drainedEntityMessages(), ElementsAre(EntityMessage{event}));
}

/**
 * Given a game lost to the silence of its server
 * When the server comes back from a stall and a WorldState of the lost session
 * arrives
 * Then the connection hands nothing over: the client left that game
 */
TEST_F(EntityMessageQueueFixture, KeepsNothingFromAServerThatComesBack) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  update();
  update(PROTOCOL_SILENCE_LIMIT);

  deliverWorldStateInSession(
      stateListing(APPLIED_TICK,
                   {listed(SHIP_ID, network::WireEntityType::PlayerShip,
                           FIRST_POSITION)}),
      SECOND_SEQUENCE);
  update();

  EXPECT_THAT(drainedEntityMessages(), IsEmpty());
}

/**
 * Given a game in progress
 * When the server ends it, then repeats the end
 * Then the game gets the end once
 */
TEST_F(EntityMessageQueueFixture, HandsOverTheEndOfTheGameOnce) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  deliverInSession(endOfOnePilot(), SECOND_SEQUENCE);
  deliverInSession(endOfOnePilot(), THIRD_SEQUENCE);

  update();

  EXPECT_THAT(drainedEntityMessages(),
              ElementsAre(EntityMessage{endOfOnePilot()}));
}

/**
 * Given a game started
 * When the server says twice that a player left
 * Then the game screen gets both copies, in order
 */
TEST_F(EntityMessageQueueFixture, HandsOverEveryCopyOfAPlayerLeft) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  const network::PlayerLeft departure{
      .playerNumber = OTHER_PLAYER_NUMBER,
      .reason = network::DepartureReason::TimedOut};
  deliverInSession(departure, SECOND_SEQUENCE);
  deliverInSession(departure, THIRD_SEQUENCE);

  update();

  EXPECT_THAT(drainedEntityMessages(),
              ElementsAre(EntityMessage{departure}, EntityMessage{departure}));
}

}  // namespace rtype::client
