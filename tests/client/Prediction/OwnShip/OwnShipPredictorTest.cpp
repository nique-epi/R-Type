#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ActionSet.hpp"
#include "EntityIdConstants.hpp"
#include "OwnShipPredictor.hpp"
#include "PlayerAction.hpp"
#include "PlayerCommand.hpp"
#include "PredictedCommand.hpp"
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"
#include "ProtocolConstants.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "ShipControl.hpp"
#include "TimeConstants.hpp"

using rtype::client::ActionSet;
using rtype::client::OwnShipPredictor;
using rtype::client::PlayerAction;
using rtype::client::PlayerCommand;
using rtype::client::PredictedCommand;
using rtype::client::testing::FIRST_COMMAND;
using rtype::client::testing::HALF_A_TICK;
using rtype::client::testing::HOLDING_RIGHT;
using rtype::client::testing::NEWER_SHIP_ID;
using rtype::client::testing::OTHER_PLAYER_NUMBER;
using rtype::client::testing::OWN_PLAYER_NUMBER;
using rtype::client::testing::OWN_SHIP_ID;
using rtype::client::testing::shipAfterCommands;
using rtype::client::testing::SHOWN_TICK;
using rtype::client::testing::START;
using rtype::client::testing::stateWithoutAShip;
using rtype::client::testing::stateWithoutEntities;
using rtype::client::testing::stateWithShip;
using rtype::client::testing::THREE_TICKS;
using rtype::client::testing::TICKS_OF_A_STALL;
using rtype::client::testing::TWO_COMMANDS;
using rtype::engine::MAXIMUM_TICKS_PER_ADVANCE;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::identifiers::NO_ENTITY_ID;
using rtype::network::FIRST_INPUT_NUMBER;

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

  predictor.advance(SIMULATION_TICK_DURATION * 2, PlayerCommand{.held = held},
                    link);

  EXPECT_EQ(link.sentActions().size(), 2U);
  EXPECT_GT(predictor.position().x, START.x);
}

/**
 * Given a predictor that read a state listing its ship, its client showing
 * tick 300
 * When it advances by two ticks
 * Then both inputs sent carry tick 300
 */
TEST(OwnShipPredictor, SendsTheTickItsClientShows) {
  RecordingPlayerInputLink link;
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));

  static_cast<void>(predictor.advance(
      SIMULATION_TICK_DURATION * 2,
      PlayerCommand{.held = {}, .shownTick = SHOWN_TICK}, link));

  EXPECT_EQ(link.shownTicks(),
            (std::vector<std::uint32_t>{SHOWN_TICK, SHOWN_TICK}));
}

/**
 * Given a predictor that read a state listing its ship
 * When it advances by one tick holding right and shoot
 * Then it gives back that command, shooting, with the ship as moved by it
 */
TEST(OwnShipPredictor, GivesBackEachCommandWithTheShipAfterIt) {
  RecordingPlayerInputLink link;
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));
  ActionSet held;
  held.add(PlayerAction::Right);
  held.add(PlayerAction::Shoot);

  const std::vector<PredictedCommand> commands = predictor.advance(
      SIMULATION_TICK_DURATION, PlayerCommand{.held = held}, link);

  ASSERT_EQ(commands.size(), 1U);
  EXPECT_EQ(commands.front().inputNumber, FIRST_INPUT_NUMBER);
  EXPECT_TRUE(commands.front().shooting);
  EXPECT_FLOAT_EQ(commands.front().shipAfter.x, predictor.position().x);
  EXPECT_GT(commands.front().shipAfter.x, START.x);
}

/**
 * Given a predictor advanced by half a tick
 * When it advances by another half
 * Then the input goes out at the second half, not before
 */
TEST(OwnShipPredictor, CountsTheTimeOfSeveralUpdatesIntoWholeTicks) {
  RecordingPlayerInputLink link;
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.advance(HALF_A_TICK, PlayerCommand{}, link);
  ASSERT_TRUE(link.sentActions().empty());

  predictor.advance(HALF_A_TICK, PlayerCommand{}, link);

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

  predictor.advance(SIMULATION_TICK_DURATION * TICKS_OF_A_STALL,
                    PlayerCommand{}, link);
  predictor.advance(HALF_A_TICK, PlayerCommand{}, link);

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
 * When a state lists the player without a ship, as once it was destroyed
 * Then it follows no ship
 */
TEST(OwnShipPredictor, ForgetsTheShipOnceThePlayerHasNone) {
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));

  predictor.apply(stateWithoutEntities(OWN_PLAYER_NUMBER, NO_ENTITY_ID));

  EXPECT_FALSE(predictor.shipEntityId().has_value());
}

/**
 * Given a predictor that follows a ship
 * When a part of a state lists the player with that ship and no entity, as a
 * state cut into several datagrams does
 * Then it still follows the ship
 */
TEST(OwnShipPredictor, KeepsTheShipThroughAPartWithoutItsEntity) {
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));

  predictor.apply(stateWithoutEntities(OWN_PLAYER_NUMBER, OWN_SHIP_ID));

  EXPECT_EQ(predictor.shipEntityId(), OWN_SHIP_ID);
}

/**
 * Given a predictor whose ship was destroyed, which advanced three ticks
 * holding right while it had none
 * When a state lists the new ship at rest at START, the server having applied
 * command 1
 * Then the ship is where commands 2 and 3 take it from START, not at START:
 * no snap back once the server applies them
 */
TEST(OwnShipPredictor, ANewShipStartsWithTheCommandsSentSince) {
  RecordingPlayerInputLink link;
  OwnShipPredictor predictor(OWN_PLAYER_NUMBER);
  predictor.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));
  predictor.apply(stateWithoutAShip(OWN_PLAYER_NUMBER, 0));
  ActionSet held;
  held.add(PlayerAction::Right);
  predictor.advance(THREE_TICKS, PlayerCommand{.held = held}, link);

  predictor.apply(
      stateWithShip(OWN_PLAYER_NUMBER, NEWER_SHIP_ID, START, FIRST_COMMAND));

  const rtype::game::MovingShip server = shipAfterCommands(
      HOLDING_RIGHT, static_cast<std::uint32_t>(TWO_COMMANDS));
  EXPECT_FLOAT_EQ(predictor.position().x, server.position.x);
}
