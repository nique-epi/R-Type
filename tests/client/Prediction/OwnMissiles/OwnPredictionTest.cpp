#include <gtest/gtest.h>
#include "EntityIdConstants.hpp"
#include "OwnPredictionFixture.hpp"
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"

using rtype::client::testing::NEWER_SHIP_ID;
using rtype::client::testing::OWN_PLAYER_NUMBER;
using rtype::client::testing::OwnPredictionFixture;
using rtype::client::testing::START;
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

  EXPECT_EQ(drawnMissileCount(), 1U);
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

  EXPECT_EQ(drawnMissileCount(), 0U);
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

  EXPECT_EQ(drawnMissileCount(), 1U);
}

/**
 * Given the prediction of a player whose ship a state listed
 * When the game ends, then it advances one tick holding shoot
 * Then no missile is drawn: the server runs nothing once a game is over
 */
TEST_F(OwnPredictionFixture, NothingFiresOnceTheGameEnded) {
  prediction().ceaseFire();

  shootOneTick();

  EXPECT_EQ(drawnMissileCount(), 0U);
}
