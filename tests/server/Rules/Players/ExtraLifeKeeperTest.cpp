#include <gtest/gtest.h>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "ExtraLifeKeeperFixture.hpp"
#include "ExtraLifeKeeperTestConstants.hpp"
#include "ExtraLifeWon.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "Lives.hpp"
#include "PlayerLives.hpp"

namespace rtype::server::testing {

/**
 * Given a player with the 3 lives of a new ship and no score
 * When it kills an enemy worth the 30000 points of the first step
 * Then its ship holds 4 lives
 */
TEST_F(ExtraLifeKeeperFixture, AKillThatReachesAStepGivesALife) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);

  kill(player, POINTS_TO_THE_FIRST_STEP);

  EXPECT_EQ(livesOf(world(), player), LIVES_WITH_ONE_WON);
}

/**
 * Given a player with the 3 lives of a new ship and no score
 * When it kills an enemy worth the 30000 points of the first step
 * Then the lives it keeps for its next ship are 4 too
 */
TEST_F(ExtraLifeKeeperFixture, TheNextShipKeepsTheLifeWon) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);

  kill(player, POINTS_TO_THE_FIRST_STEP);

  EXPECT_EQ(player.livesWithoutShip, LIVES_WITH_ONE_WON);
}

/**
 * Given a player with no score
 * When it kills an enemy worth the 30000 points of the first step
 * Then the extra life of that player is announced, once
 */
TEST_F(ExtraLifeKeeperFixture, AnnouncesTheLifeWon) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);

  kill(player, POINTS_TO_THE_FIRST_STEP);

  EXPECT_EQ(wins(), std::vector{game::ExtraLifeWon{.playerNumber =
                                                       player.playerNumber}});
}

/**
 * Given a player with no score
 * When it kills a Drifter, far under the first step
 * Then it keeps its 3 lives and nothing is announced
 */
TEST_F(ExtraLifeKeeperFixture, AKillUnderTheStepGivesNothing) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);

  kill(player, POINTS_OF_A_DRIFTER);

  EXPECT_EQ(livesOf(world(), player), LIVES_OF_A_NEW_SHIP);
  EXPECT_TRUE(wins().empty());
}

/**
 * Given a player who holds the 9 lives a player may hold at most
 * When it reaches the first step
 * Then it keeps 9 lives, and nothing is announced
 */
TEST_F(ExtraLifeKeeperFixture, NeverGivesMoreThanTheMostLives) {
  ConnectedPlayer& player = join(FIRST_PLAYER);
  player.livesWithoutShip = MOST_LIVES;
  world().shipComponentOf<game::Lives>(player.shipEntityId)->count = MOST_LIVES;

  kill(player, POINTS_TO_THE_FIRST_STEP);

  EXPECT_EQ(livesOf(world(), player), MOST_LIVES);
  EXPECT_TRUE(wins().empty());
}

/**
 * Given a player out of lives, without a ship, watching
 * When a missile it fired before reaches the first step
 * Then it stays out of lives: an extra life never brings a player back
 */
TEST_F(ExtraLifeKeeperFixture, APlayerOutOfLivesWinsNone) {
  ConnectedPlayer& player = join(FIRST_PLAYER);
  static_cast<void>(world().removePlayer(player.shipEntityId));
  player.shipEntityId = identifiers::NO_ENTITY_ID;
  player.livesWithoutShip = NO_LIVES;

  kill(player, POINTS_TO_THE_FIRST_STEP);

  EXPECT_EQ(livesOf(world(), player), NO_LIVES);
}

}  // namespace rtype::server::testing
