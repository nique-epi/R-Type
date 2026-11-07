#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "OwnMissilesFixture.hpp"
#include "OwnMissilesTestConstants.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SoundConstants.hpp"

using rtype::client::PLAYER_SHOT_SOUND_ID;
using rtype::client::testing::FIRING_INPUT;
using rtype::client::testing::INPUT_STILL_WAITING_FOR_IT;
using rtype::client::testing::INPUT_THAT_REFUSES_THE_FIRST_SHOT;
using rtype::client::testing::MISSILE_AT_THE_MUZZLE;
using rtype::client::testing::OwnMissilesFixture;
using rtype::client::testing::SERVER_MISSILE_ID;
using rtype::client::testing::SERVER_START;
using rtype::client::testing::SHIP_WHEN_IT_FIRES;
using rtype::client::testing::SHOOTING_PLAYER;
using rtype::client::testing::THREE_TICKS_LATER_INPUT;
using rtype::client::testing::THREE_TICKS_OF_FLIGHT;
using rtype::network::DestructionReason;
using rtype::network::EntityDestroyed;
using rtype::network::EntitySpawned;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given the missiles of a player whose gun is ready
 * When a command holding shoot is sent, the predicted ship at (200, 300)
 * Then one missile is drawn at once at the muzzle, 96 units ahead, and the
 * shot of a player plays
 */
TEST_F(OwnMissilesFixture, AShotLeavesTheMuzzleOfThePredictedShip) {
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  ASSERT_EQ(drawnMissiles().size(), 1U);
  EXPECT_FLOAT_EQ(drawnMissiles().front().x, MISSILE_AT_THE_MUZZLE.x);
  EXPECT_FLOAT_EQ(drawnMissiles().front().y, MISSILE_AT_THE_MUZZLE.y);
  EXPECT_THAT(sounds().played(), ElementsAre(PLAYER_SHOT_SOUND_ID));
}

/**
 * Given a missile fired by input 1
 * When the commands up to input 4 are sent
 * Then it is drawn three ticks of flight, 48 units, further: on the timeline
 * of the predicted ship
 */
TEST_F(OwnMissilesFixture, AMissileFliesOnTheTimelineOfTheShip) {
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  holdNothingUpTo(THREE_TICKS_LATER_INPUT);

  ASSERT_EQ(drawnMissiles().size(), 1U);
  EXPECT_FLOAT_EQ(drawnMissiles().front().x,
                  MISSILE_AT_THE_MUZZLE.x + THREE_TICKS_OF_FLIGHT);
}

/**
 * Given a missile fired by input 1
 * When the server says it fired input 1 from (300, 300)
 * Then the same missile flies from where the server put it, and no second
 * one appears
 */
TEST_F(OwnMissilesFixture, TheMissileOfTheServerTakesTheOneFired) {
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  missiles().ownMissileSpawned(EntitySpawned{.entityId = SERVER_MISSILE_ID,
                                             .entityType = 3,
                                             .x = SERVER_START.x,
                                             .y = SERVER_START.y,
                                             .owner = SHOOTING_PLAYER,
                                             .firingInput = FIRING_INPUT});
  holdNothingUpTo(THREE_TICKS_LATER_INPUT);

  ASSERT_EQ(drawnMissiles().size(), 1U);
  EXPECT_FLOAT_EQ(drawnMissiles().front().x,
                  SERVER_START.x + THREE_TICKS_OF_FLIGHT);
}

/**
 * Given a gun that fired nothing on input 1, its player holding nothing
 * When the server says it fired input 1
 * Then the missile appears where it flies, three ticks after its input
 */
TEST_F(OwnMissilesFixture, AShotTheClientMissedAppearsWhereItFlies) {
  holdNothingUpTo(THREE_TICKS_LATER_INPUT);

  missiles().ownMissileSpawned(EntitySpawned{.entityId = SERVER_MISSILE_ID,
                                             .entityType = 3,
                                             .x = SERVER_START.x,
                                             .y = SERVER_START.y,
                                             .owner = SHOOTING_PLAYER,
                                             .firingInput = FIRING_INPUT});

  ASSERT_EQ(drawnMissiles().size(), 1U);
  EXPECT_FLOAT_EQ(drawnMissiles().front().x,
                  SERVER_START.x + THREE_TICKS_OF_FLIGHT);
}

/**
 * Given a missile fired by input 1 the server has not answered
 * When the server applied the inputs up to 14
 * Then it is still drawn: its EntitySpawned may still be on its way
 */
TEST_F(OwnMissilesFixture, AShotWaitsForTheAnswerOfTheServer) {
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  missiles().acknowledge(INPUT_STILL_WAITING_FOR_IT);

  EXPECT_EQ(drawnMissiles().size(), 1U);
}

/**
 * Given a missile fired by input 1 the server has not answered
 * When the server applied the inputs up to 15, fourteen past it
 * Then it is taken away: the server refused that shot
 */
TEST_F(OwnMissilesFixture, ARefusedShotIsTakenAway) {
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);

  missiles().acknowledge(INPUT_THAT_REFUSES_THE_FIRST_SHOT);

  EXPECT_THAT(drawnMissiles(), IsEmpty());
}

/**
 * Given a missile drawn on an enemy drawn
 * When the missiles test what they touch
 * Then the missile is gone
 */
TEST_F(OwnMissilesFixture, AMissileThatTouchesAnEnemyIsGone) {
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);
  enemyAt(MISSILE_AT_THE_MUZZLE);

  missiles().touchEnemies();

  EXPECT_THAT(drawnMissiles(), IsEmpty());
}

/**
 * Given a missile gone on an enemy before the server answered
 * When the server says it fired that missile
 * Then nothing is drawn again: a missile gone is never shown again
 */
TEST_F(OwnMissilesFixture, AMissileGoneIsNeverDrawnAgain) {
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);
  enemyAt(MISSILE_AT_THE_MUZZLE);
  missiles().touchEnemies();

  missiles().ownMissileSpawned(EntitySpawned{.entityId = SERVER_MISSILE_ID,
                                             .entityType = 3,
                                             .x = SERVER_START.x,
                                             .y = SERVER_START.y,
                                             .owner = SHOOTING_PLAYER,
                                             .firingInput = FIRING_INPUT});

  EXPECT_THAT(drawnMissiles(), IsEmpty());
}

/**
 * Given a missile the server answered
 * When the server removes it
 * Then it is taken away
 */
TEST_F(OwnMissilesFixture, TheServerRemovingAMissileTakesItAway) {
  fireAt(FIRING_INPUT, SHIP_WHEN_IT_FIRES);
  missiles().ownMissileSpawned(EntitySpawned{.entityId = SERVER_MISSILE_ID,
                                             .entityType = 3,
                                             .x = SERVER_START.x,
                                             .y = SERVER_START.y,
                                             .owner = SHOOTING_PLAYER,
                                             .firingInput = FIRING_INPUT});

  missiles().ownMissileDestroyed(EntityDestroyed{
      .entityId = SERVER_MISSILE_ID, .reason = DestructionReason::Killed});

  EXPECT_THAT(drawnMissiles(), IsEmpty());
}
