#include <gtest/gtest.h>
#include "EntityIdConstants.hpp"
#include "OwnMissilesTestConstants.hpp"
#include "OwnPredictionFixture.hpp"
#include "OwnPredictionTestConstants.hpp"
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"

using rtype::client::testing::APPLIED_BEFORE_THE_NEW_SHIP;
using rtype::client::testing::INPUT_THAT_REFUSES_THE_FIRST_SHOT;
using rtype::client::testing::MISSILE_OF_THE_NEW_SHIP_AT_INPUT_THREE;
using rtype::client::testing::NEWER_SHIP_ID;
using rtype::client::testing::NOTHING_ACKNOWLEDGED;
using rtype::client::testing::OWN_PLAYER_NUMBER;
using rtype::client::testing::OwnPredictionFixture;
using rtype::client::testing::START;
using rtype::client::testing::stateWithoutAShip;
using rtype::client::testing::stateWithoutEntities;
using rtype::client::testing::stateWithShip;
using rtype::identifiers::NO_ENTITY_ID;

/**
 * Given the prediction of a player whose ship a state listed
 * When it advances one tick holding shoot
 * Then one missile is drawn
 */
TEST_F(OwnPredictionFixture, AShipStillListedFires) {
  shootOneTick();

  EXPECT_EQ(drawnMissiles().size(), 1U);
}

/**
 * Given the prediction of a player whose ship a state listed
 * When a state lists the player without a ship, as once it was destroyed,
 * then the prediction advances one tick holding shoot
 * Then no missile is drawn: the server fires nothing for a ship it destroyed
 */
TEST_F(OwnPredictionFixture, ADestroyedShipFiresNothing) {
  apply(stateWithoutEntities(OWN_PLAYER_NUMBER, NO_ENTITY_ID));

  shootOneTick();

  EXPECT_EQ(drawnMissiles().size(), 0U);
}

/**
 * Given the prediction of a player whose ship was destroyed
 * When a state lists its new ship, then it advances one tick holding shoot
 * Then one missile is drawn, from the new ship
 */
TEST_F(OwnPredictionFixture, ANewShipFiresAgain) {
  apply(stateWithoutEntities(OWN_PLAYER_NUMBER, NO_ENTITY_ID));
  apply(stateWithShip(OWN_PLAYER_NUMBER, NEWER_SHIP_ID, START, 0));

  shootOneTick();

  EXPECT_EQ(drawnMissiles().size(), 1U);
}

/**
 * Given the prediction of a player whose ship a state listed
 * When the game ends, then it advances one tick holding shoot
 * Then no missile is drawn: the server runs nothing once a game is over
 */
TEST_F(OwnPredictionFixture, NothingFiresOnceTheGameEnded) {
  prediction().ceaseFire();

  shootOneTick();

  EXPECT_EQ(drawnMissiles().size(), 0U);
}

/**
 * Given the prediction of a player whose ship fired a missile
 * When a state lists the player without a ship, then the prediction advances
 * one more tick
 * Then the missile is still drawn, further right: it flies on as the
 * missile of the server does
 */
TEST_F(OwnPredictionFixture, AMissileFliesOnOnceItsShipIsDestroyed) {
  shootOneTick();
  const float firedAt = drawnMissiles().front().x;
  apply(stateWithoutEntities(OWN_PLAYER_NUMBER, NO_ENTITY_ID));

  shootOneTick();

  ASSERT_EQ(drawnMissiles().size(), 1U);
  EXPECT_GT(drawnMissiles().front().x, firedAt);
}

/**
 * Given the prediction of a player whose ship fired a missile
 * When the game ends, then the prediction advances one more tick
 * Then the missile is still drawn, further right
 */
TEST_F(OwnPredictionFixture, AMissileFliesOnOnceTheGameEnded) {
  shootOneTick();
  const float firedAt = drawnMissiles().front().x;
  prediction().ceaseFire();

  shootOneTick();

  ASSERT_EQ(drawnMissiles().size(), 1U);
  EXPECT_GT(drawnMissiles().front().x, firedAt);
}

/**
 * Given the prediction of a player whose ship was destroyed, three commands
 * holding shoot sent while it had none
 * When a state lists its new ship at (400, 300), the server having applied
 * input 1
 * Then the command of input 2 fires from the new ship, as the server fires
 * it: one missile, one tick of flight ahead of the muzzle
 */
TEST_F(OwnPredictionFixture, ANewShipFiresTheShotsSentBeforeItsState) {
  apply(stateWithoutAShip(OWN_PLAYER_NUMBER, NOTHING_ACKNOWLEDGED));
  shootOneTick();
  shootOneTick();
  shootOneTick();

  apply(stateWithShip(OWN_PLAYER_NUMBER, NEWER_SHIP_ID, START,
                      APPLIED_BEFORE_THE_NEW_SHIP));

  ASSERT_EQ(drawnMissiles().size(), 1U);
  EXPECT_FLOAT_EQ(drawnMissiles().front().x,
                  MISSILE_OF_THE_NEW_SHIP_AT_INPUT_THREE);
}

/**
 * Given the prediction of a player whose ship fired a missile by input 1
 * When a state lists the player without a ship, the server having applied
 * inputs up to 15
 * Then the missile is taken away: the server refused that shot
 */
TEST_F(OwnPredictionFixture, ARefusedShotIsTakenAwayWithoutAShip) {
  shootOneTick();

  apply(
      stateWithoutAShip(OWN_PLAYER_NUMBER, INPUT_THAT_REFUSES_THE_FIRST_SHOT));

  EXPECT_TRUE(drawnMissiles().empty());
}
