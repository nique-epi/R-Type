#include <gtest/gtest.h>
#include "ActionSet.hpp"
#include "OwnShipPredictor.hpp"
#include "PlayerAction.hpp"
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "TimeConstants.hpp"

using rtype::client::ActionSet;
using rtype::client::OwnShipPredictor;
using rtype::client::PlayerAction;
using rtype::client::testing::HALF_A_TICK;
using rtype::client::testing::NEWER_SHIP_ID;
using rtype::client::testing::OTHER_PLAYER_NUMBER;
using rtype::client::testing::OWN_PLAYER_NUMBER;
using rtype::client::testing::OWN_SHIP_ID;
using rtype::client::testing::START;
using rtype::client::testing::stateWithoutEntities;
using rtype::client::testing::stateWithShip;
using rtype::client::testing::TICKS_OF_A_STALL;
using rtype::engine::MAXIMUM_TICKS_PER_ADVANCE;
using rtype::engine::SIMULATION_TICK_DURATION;

/**
 * Given a predictor that read a state listing its ship
 * When it advances by two ticks holding right
 * Then it sent two inputs and the ship moved right of the start
 */
TEST(OwnShipPredictor, SendsOneInputPerTickAndMovesTheShip) {
  RecordingPlayerInputLink link;
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));
  ActionSet held;
  held.add(PlayerAction::Right);

  predictor.advance(SIMULATION_TICK_DURATION * 2, held, link);

  EXPECT_EQ(link.sentActions().size(), 2U);
  EXPECT_GT(predictor.position().x, START.x);
}

/**
 * Given a predictor advanced by half a tick
 * When it advances by another half
 * Then the input goes out at the second half, not before
 */
TEST(OwnShipPredictor, CountsTheTimeOfSeveralUpdatesIntoWholeTicks) {
  RecordingPlayerInputLink link;
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.advance(HALF_A_TICK, ActionSet{}, link);
  ASSERT_TRUE(link.sentActions().empty());

  predictor.advance(HALF_A_TICK, ActionSet{}, link);

  EXPECT_EQ(link.sentActions().size(), 1U);
}

/**
 * Given a predictor
 * When it advances by a stall of ten ticks
 * Then it sends the most it may catch up, and the rest of the time is dropped
 */
TEST(OwnShipPredictor, CatchesUpAtMostTheTicksOfOneAdvance) {
  RecordingPlayerInputLink link;
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);

  predictor.advance(SIMULATION_TICK_DURATION * TICKS_OF_A_STALL, ActionSet{},
                    link);
  predictor.advance(HALF_A_TICK, ActionSet{}, link);

  EXPECT_EQ(link.sentActions().size(), MAXIMUM_TICKS_PER_ADVANCE);
}

/**
 * Given a predictor that read no state yet
 * When a state lists its ship
 * Then it knows the entity of the ship and starts at the position of the state
 */
TEST(OwnShipPredictor, StartsFromTheFirstStateThatListsTheShip) {
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);

  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));

  EXPECT_EQ(predictor.shipEntityId(), OWN_SHIP_ID);
  EXPECT_FLOAT_EQ(predictor.position().x, START.x);
}

/**
 * Given a predictor
 * When it reads a state of another player and a part with no entity
 * Then it still has no ship
 */
TEST(OwnShipPredictor, WaitsForAStateThatListsTheShip) {
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);

  predictor.apply(stateWithShip(OTHER_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));
  predictor.apply(stateWithoutEntities(OWN_PLAYER_NUMBER, OWN_SHIP_ID));

  EXPECT_FALSE(predictor.shipEntityId().has_value());
}

/**
 * Given a predictor that follows a ship
 * When a state lists the player with another ship, as after a hit
 * Then it follows the new ship
 */
TEST(OwnShipPredictor, FollowsTheNewShipOfAPlayerWhoCameBack) {
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));

  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, NEWER_SHIP_ID, START, 0));

  EXPECT_EQ(predictor.shipEntityId(), NEWER_SHIP_ID);
}

/**
 * Given a predictor that follows a ship
 * When it is stopped
 * Then it follows no ship
 */
TEST(OwnShipPredictor, StopsFollowingTheShipWhenStopped) {
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));

  predictor.stop();

  EXPECT_FALSE(predictor.shipEntityId().has_value());
}
