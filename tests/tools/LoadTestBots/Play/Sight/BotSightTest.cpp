#include <gtest/gtest.h>
#include "BotSight.hpp"
#include "BotSightFixtures.hpp"
#include "BotSightTestConstants.hpp"
#include "EntityKind.hpp"
#include "FixedEntityTypeTable.hpp"

using rtype::tools::BotSight;
using rtype::tools::EntityKind;
using rtype::tools::testing::endOfGame;
using rtype::tools::testing::ENEMY_MISSILE_TYPE;
using rtype::tools::testing::ENEMY_TYPE;
using rtype::tools::testing::entityAt;
using rtype::tools::testing::FixedEntityTypeTable;
using rtype::tools::testing::NO_LIFE;
using rtype::tools::testing::NO_SUCH_TYPE;
using rtype::tools::testing::PLAYER_MISSILE_TYPE;
using rtype::tools::testing::SIGHT_PLAYER;
using rtype::tools::testing::SPEED_TO_THE_LEFT;
using rtype::tools::testing::START_X;
using rtype::tools::testing::stateOf;
using rtype::tools::testing::THREE_HUNDRED_POINTS;
using rtype::tools::testing::THREE_LIVES;
using rtype::tools::testing::THREE_TICKS_LATER;
using rtype::tools::testing::TWELVE_UNITS_LEFT;
using rtype::tools::testing::TWO_HUNDRED_POINTS;
using rtype::tools::testing::TWO_LIVES;

/**
 * Given an enemy seen at one tick
 * When a state three ticks later shows it 12 units to the left
 * Then its velocity is 240 units per second to the left
 */
TEST(BotSight, ComputesAVelocityBetweenTwoTicks) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.entities = {entityAt(ENEMY_TYPE, START_X)}}),
             SIGHT_PLAYER);

  sight.read(stateOf({.tick = THREE_TICKS_LATER,
                      .entities = {entityAt(ENEMY_TYPE, TWELVE_UNITS_LEFT)}}),
             SIGHT_PLAYER);

  ASSERT_EQ(sight.entities().size(), 1U);
  EXPECT_FLOAT_EQ(sight.entities().front().velocityX, SPEED_TO_THE_LEFT);
}

/**
 * Given a state listing an enemy missile
 * When the sight reads it
 * Then it knows the missile by the side of its type
 */
TEST(BotSight, ReadsTheKindFromTheTableOfTheSession) {
  const FixedEntityTypeTable types;
  BotSight sight{types};

  sight.read(stateOf({.entities = {entityAt(ENEMY_MISSILE_TYPE, START_X)}}),
             SIGHT_PLAYER);

  ASSERT_EQ(sight.entities().size(), 1U);
  EXPECT_EQ(sight.entities().front().kind, EntityKind::EnemyMissile);
}

/**
 * Given a state listing a missile of a player and a type no table knows
 * When the sight reads it
 * Then it sees neither, and not the ship either
 */
TEST(BotSight, LeavesOutWhatABotDoesNotLookAt) {
  const FixedEntityTypeTable types;
  BotSight sight{types};

  sight.read(stateOf({.entities = {entityAt(PLAYER_MISSILE_TYPE, START_X),
                                   entityAt(NO_SUCH_TYPE, START_X)}}),
             SIGHT_PLAYER);

  EXPECT_TRUE(sight.entities().empty());
}

/**
 * Given a bot with three lives
 * When a state gives it two
 * Then one life is counted as lost
 */
TEST(BotSight, CountsALifeLost) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.lives = THREE_LIVES, .entities = {}}), SIGHT_PLAYER);

  sight.read(
      stateOf({.tick = THREE_TICKS_LATER, .lives = TWO_LIVES, .entities = {}}),
      SIGHT_PLAYER);

  EXPECT_EQ(sight.figures().livesLost, 1U);
}

/**
 * Given a bot that ended a game with no life left
 * When the next game gives it three lives
 * Then no life is counted as lost
 */
TEST(BotSight, ANewGameLosesNoLife) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.lives = NO_LIFE, .entities = {}}), SIGHT_PLAYER);
  sight.read(endOfGame(), SIGHT_PLAYER);

  sight.read(stateOf({.lives = THREE_LIVES, .entities = {}}), SIGHT_PLAYER);

  EXPECT_EQ(sight.figures().livesLost, 0U);
}

/**
 * Given a game that ended with 200 points
 * When the next game reaches 300 points
 * Then the run holds 500 points
 */
TEST(BotSight, AddsTheScoreOfEveryGame) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.score = TWO_HUNDRED_POINTS, .entities = {}}),
             SIGHT_PLAYER);
  sight.read(endOfGame(), SIGHT_PLAYER);

  sight.read(stateOf({.score = THREE_HUNDRED_POINTS, .entities = {}}),
             SIGHT_PLAYER);

  EXPECT_EQ(sight.figures().scoreEarned,
            TWO_HUNDRED_POINTS + THREE_HUNDRED_POINTS);
}

/**
 * Given a bot in play
 * When its game ends
 * Then it is no longer in play and sees nothing
 */
TEST(BotSight, TheEndOfAGameEndsThePlay) {
  const FixedEntityTypeTable types;
  BotSight sight{types};
  sight.read(stateOf({.entities = {entityAt(ENEMY_TYPE, START_X)}}),
             SIGHT_PLAYER);

  sight.read(endOfGame(), SIGHT_PLAYER);

  EXPECT_FALSE(sight.figures().inPlay);
  EXPECT_TRUE(sight.entities().empty());
}
