#include <gtest/gtest.h>
#include "OwnShip.hpp"
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"

using rtype::client::ownShipOf;
using rtype::client::testing::ACKNOWLEDGED_COMMAND;
using rtype::client::testing::OTHER_PLAYER_NUMBER;
using rtype::client::testing::OWN_PLAYER_NUMBER;
using rtype::client::testing::OWN_SHIP_ID;
using rtype::client::testing::START;
using rtype::client::testing::stateWithoutEntities;
using rtype::client::testing::stateWithShip;

/**
 * Given a state listing the player and its ship
 * When the own ship of that player is read
 * Then the ship id, the position and the confirmed input are those of the state
 */
TEST(OwnShip, ReadsTheShipOfThePlayerFromTheState) {
  const auto own = ownShipOf(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID,
                                           START, ACKNOWLEDGED_COMMAND),
                             OWN_PLAYER_NUMBER);

  ASSERT_TRUE(own.has_value());
  EXPECT_EQ(own->shipEntityId, OWN_SHIP_ID);
  EXPECT_FLOAT_EQ(own->position.x, START.x);
  EXPECT_FLOAT_EQ(own->position.y, START.y);
  EXPECT_EQ(own->acknowledgedInput, ACKNOWLEDGED_COMMAND);
}

/**
 * Given a state that lists another player only
 * When the own ship of the player is read
 * Then there is none
 */
TEST(OwnShip, FindsNothingWhenTheStateDoesNotListThePlayer) {
  const auto own =
      ownShipOf(stateWithShip(OTHER_PLAYER_NUMBER, OWN_SHIP_ID, START, 0),
                OWN_PLAYER_NUMBER);

  EXPECT_FALSE(own.has_value());
}

/**
 * Given a part of a state that lists the player but none of the entities
 * When the own ship of the player is read
 * Then there is none, since the position is in another part
 */
TEST(OwnShip, FindsNothingWhenThePartListsNoEntityForTheShip) {
  const auto own = ownShipOf(
      stateWithoutEntities(OWN_PLAYER_NUMBER, OWN_SHIP_ID), OWN_PLAYER_NUMBER);

  EXPECT_FALSE(own.has_value());
}
