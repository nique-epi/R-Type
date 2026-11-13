#include <gtest/gtest.h>
#include "ActionSet.hpp"
#include "BotSight.hpp"
#include "BotSightFixtures.hpp"
#include "BotSightTestConstants.hpp"
#include "FixedEntityTypeTable.hpp"
#include "PlayerAction.hpp"
#include "RulePilot.hpp"
#include "RulePilotTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

using rtype::client::ActionSet;
using rtype::client::PlayerAction;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::network::EntityState;
using rtype::tools::BotSight;
using rtype::tools::RulePilot;
using rtype::tools::testing::ABOVE_THE_SHIP_Y;
using rtype::tools::testing::BELOW_THE_SHIP_Y;
using rtype::tools::testing::BOSS_TYPE;
using rtype::tools::testing::ENEMY_MISSILE_TYPE;
using rtype::tools::testing::ENEMY_TYPE;
using rtype::tools::testing::FAR_ABOVE_THE_SHIP_Y;
using rtype::tools::testing::FIRST_BOT_INDEX;
using rtype::tools::testing::FixedEntityTypeTable;
using rtype::tools::testing::HIGHER_ENEMY_ID;
using rtype::tools::testing::MISSILE_NEXT_X;
using rtype::tools::testing::MISSILE_START_X;
using rtype::tools::testing::NEARER_X;
using rtype::tools::testing::OUT_OF_REACH_X;
using rtype::tools::testing::SECOND_BOT_INDEX;
using rtype::tools::testing::SEEN_ID;
using rtype::tools::testing::SHIP_Y;
using rtype::tools::testing::SIGHT_PLAYER;
using rtype::tools::testing::stateOf;
using rtype::tools::testing::THREE_TICKS_LATER;

/**
 * Given a bot out of play
 * When the pilot advances
 * Then it holds nothing
 */
TEST(RulePilot, HoldsNothingOutOfPlay) {
  const FixedEntityTypeTable types;
  const BotSight sight{types};
  RulePilot pilot{FIRST_BOT_INDEX};

  EXPECT_EQ(pilot.advance(SIMULATION_TICK_DURATION, sight).bits(),
            ActionSet{}.bits());
}

/**
 * Given a missile flying fast along the row of the ship
 * When the pilot advances
 * Then it leaves the row, since going left would not outrun it
 */
TEST(RulePilot, LeavesTheRowOfAMissileComingAtIt) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.entities = {EntityState{.entityId = SEEN_ID,
                                               .entityType = ENEMY_MISSILE_TYPE,
                                               .x = MISSILE_START_X,
                                               .y = SHIP_Y}}}),
             SIGHT_PLAYER);
  sight.read(stateOf({.tick = THREE_TICKS_LATER,
                      .entities = {EntityState{.entityId = SEEN_ID,
                                               .entityType = ENEMY_MISSILE_TYPE,
                                               .x = MISSILE_NEXT_X,
                                               .y = SHIP_Y}}}),
             SIGHT_PLAYER);
  RulePilot pilot{FIRST_BOT_INDEX};

  const ActionSet held = pilot.advance(SIMULATION_TICK_DURATION, sight);

  EXPECT_TRUE(held.has(PlayerAction::Up) || held.has(PlayerAction::Down));
}

/**
 * Given a missile flying along a row far above the ship
 * When the pilot advances
 * Then it stays on its row
 */
TEST(RulePilot, StaysOnItsRowWhenAMissilePassesFarAway) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.entities = {EntityState{.entityId = SEEN_ID,
                                               .entityType = ENEMY_MISSILE_TYPE,
                                               .x = MISSILE_START_X,
                                               .y = FAR_ABOVE_THE_SHIP_Y}}}),
             SIGHT_PLAYER);
  sight.read(stateOf({.tick = THREE_TICKS_LATER,
                      .entities = {EntityState{.entityId = SEEN_ID,
                                               .entityType = ENEMY_MISSILE_TYPE,
                                               .x = MISSILE_NEXT_X,
                                               .y = FAR_ABOVE_THE_SHIP_Y}}}),
             SIGHT_PLAYER);
  RulePilot pilot{FIRST_BOT_INDEX};

  const ActionSet held = pilot.advance(SIMULATION_TICK_DURATION, sight);

  EXPECT_FALSE(held.has(PlayerAction::Up) || held.has(PlayerAction::Down));
}

/**
 * Given an enemy far ahead, below the ship
 * When the pilot advances
 * Then it joins the row of that enemy, firing
 */
TEST(RulePilot, JoinsTheRowOfAFarEnemy) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.entities = {EntityState{.entityId = SEEN_ID,
                                               .entityType = ENEMY_TYPE,
                                               .x = OUT_OF_REACH_X,
                                               .y = BELOW_THE_SHIP_Y}}}),
             SIGHT_PLAYER);
  RulePilot pilot{FIRST_BOT_INDEX};

  const ActionSet held = pilot.advance(SIMULATION_TICK_DURATION, sight);

  EXPECT_TRUE(held.has(PlayerAction::Down));
  EXPECT_TRUE(held.has(PlayerAction::Shoot));
}

/**
 * Given two enemies, one above and one below the ship
 * When the pilots of two bots advance
 * Then each goes for its own
 */
TEST(RulePilot, TwoBotsShareTheEnemies) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.entities = {EntityState{.entityId = SEEN_ID,
                                               .entityType = ENEMY_TYPE,
                                               .x = OUT_OF_REACH_X,
                                               .y = ABOVE_THE_SHIP_Y},
                                   EntityState{.entityId = HIGHER_ENEMY_ID,
                                               .entityType = ENEMY_TYPE,
                                               .x = OUT_OF_REACH_X,
                                               .y = BELOW_THE_SHIP_Y}}}),
             SIGHT_PLAYER);
  RulePilot first{FIRST_BOT_INDEX};
  RulePilot second{SECOND_BOT_INDEX};

  EXPECT_TRUE(
      first.advance(SIMULATION_TICK_DURATION, sight).has(PlayerAction::Up));
  EXPECT_TRUE(
      second.advance(SIMULATION_TICK_DURATION, sight).has(PlayerAction::Down));
}

/**
 * Given a boss above the ship and an enemy of a lower id below it
 * When the pilot advances
 * Then it goes for the boss
 */
TEST(RulePilot, TheBossComesFirst) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.entities = {EntityState{.entityId = SEEN_ID,
                                               .entityType = ENEMY_TYPE,
                                               .x = NEARER_X,
                                               .y = BELOW_THE_SHIP_Y},
                                   EntityState{.entityId = HIGHER_ENEMY_ID,
                                               .entityType = BOSS_TYPE,
                                               .x = OUT_OF_REACH_X,
                                               .y = ABOVE_THE_SHIP_Y}}}),
             SIGHT_PLAYER);
  RulePilot pilot{FIRST_BOT_INDEX};

  EXPECT_TRUE(
      pilot.advance(SIMULATION_TICK_DURATION, sight).has(PlayerAction::Up));
}
