#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "GameInstanceTestConstants.hpp"
#include "LifeKeeperFixture.hpp"
#include "LifeKeeperTestConstants.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"

namespace rtype::server::testing {

/**
 * Given a player with three lives
 * When its ship touches a Bydo
 * Then the player has one life less
 */
TEST_F(LifeKeeperFixture, TakesALifeWhenItsShipTouchesABydo) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);

  touchABydo(player);

  EXPECT_EQ(player.lives, THREE_LIVES - 1);
}

/**
 * Given a player with a ship
 * When its ship touches a Bydo
 * Then the ship and the Bydo both explode, once each
 */
TEST_F(LifeKeeperFixture, DestroysTheShipAndTheBydoWithAnExplosion) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  const std::uint32_t ship = player.shipEntityId;

  const std::uint32_t bydo = touchABydo(player);

  EXPECT_EQ(explosionsOf(ship), 1U);
  EXPECT_EQ(explosionsOf(bydo), 1U);
}

/**
 * Given two players
 * When the ship of the second one touches a Bydo
 * Then the bus hears that the second player was hit, and nobody else
 */
TEST_F(LifeKeeperFixture, AnnouncesWhichPlayerWasHit) {
  join(FIRST_PLAYER);
  const ConnectedPlayer& second = join(SECOND_PLAYER);

  touchABydo(second);

  EXPECT_EQ(hits(),
            std::vector{game::PlayerHit{.playerNumber = second.playerNumber}});
}

/**
 * Given a player on its last life
 * When its ship touches a Bydo
 * Then the bus hears that this player is eliminated
 */
TEST_F(LifeKeeperFixture, EliminatesThePlayerAtItsLastLife) {
  ConnectedPlayer& player = join(FIRST_PLAYER);
  player.lives = ONE_LIFE;

  touchABydo(player);

  EXPECT_EQ(eliminations(), std::vector{game::PlayerElimination{
                                .playerNumber = player.playerNumber}});
}

/**
 * Given a player whose ship touched a Bydo
 * When the ticks of one second but one run
 * Then the player still has no ship
 */
TEST_F(LifeKeeperFixture, KeepsThePlayerWithoutAShipForOneSecond) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  touchABydo(player);

  runTicks(ONE_SECOND_IN_TICKS - 1);

  EXPECT_EQ(player.shipEntityId, identifiers::NO_ENTITY_ID);
}

/**
 * Given a player whose ship touched a Bydo
 * When the ticks of one second run
 * Then the player has a new ship, under a new network id
 */
TEST_F(LifeKeeperFixture, GivesANewShipAfterOneSecond) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  const std::uint32_t firstShip = player.shipEntityId;
  touchABydo(player);

  runTicks(ONE_SECOND_IN_TICKS);

  EXPECT_NE(player.shipEntityId, identifiers::NO_ENTITY_ID);
  EXPECT_NE(player.shipEntityId, firstShip);
}

/**
 * Given a player whose last life was lost
 * When the ticks of one second run
 * Then the player still has no ship: it watches
 */
TEST_F(LifeKeeperFixture, NeverBringsBackTheShipOfAPlayerWithNoLifeLeft) {
  ConnectedPlayer& player = join(FIRST_PLAYER);
  player.lives = ONE_LIFE;
  touchABydo(player);

  runTicks(ONE_SECOND_IN_TICKS);

  EXPECT_EQ(player.shipEntityId, identifiers::NO_ENTITY_ID);
}

/**
 * Given a ship that came back after a hit, on the last tick of its two seconds
 * of protection
 * When it touches a Bydo
 * Then the player keeps its lives
 */
TEST_F(LifeKeeperFixture, ProtectsTheNewShipForTwoSeconds) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  touchABydo(player);
  runTicks(ONE_SECOND_IN_TICKS);
  runTicks(TWO_SECONDS_IN_TICKS - 1);
  const std::uint8_t livesBefore = player.lives;

  touchABydo(player);

  EXPECT_EQ(player.lives, livesBefore);
}

/**
 * Given a ship that came back after a hit two seconds ago
 * When it touches a Bydo
 * Then the player loses a life
 */
TEST_F(LifeKeeperFixture, HurtsTheNewShipOnceTwoSecondsPassed) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  touchABydo(player);
  runTicks(ONE_SECOND_IN_TICKS);
  runTicks(TWO_SECONDS_IN_TICKS);
  const std::uint8_t livesBefore = player.lives;

  touchABydo(player);

  EXPECT_EQ(player.lives, livesBefore - 1);
}

/**
 * Given a ship touching two Bydos
 * When both contacts are delivered in one dispatch
 * Then the player loses one life
 */
TEST_F(LifeKeeperFixture, CountsOneLifeForTwoBydosInOneDispatch) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  const std::uint32_t firstBydo = bydoOn(player);
  const std::uint32_t secondBydo = bydoOn(player);
  publishContact(player.shipEntityId, firstBydo);
  publishContact(player.shipEntityId, secondBydo);

  deliver();

  EXPECT_EQ(player.lives, THREE_LIVES - 1);
}

/**
 * Given a player with a ship and no life, which no rule of the game leads to
 * When its ship touches a Bydo
 * Then it still has no life: the count never wraps around
 */
TEST_F(LifeKeeperFixture, NeverTakesALifeFromAPlayerWithNone) {
  ConnectedPlayer& player = join(FIRST_PLAYER);
  player.lives = 0;

  touchABydo(player);

  EXPECT_EQ(player.lives, 0);
}

/**
 * Given a keeper that was destroyed
 * When the ship of a player touches a Bydo
 * Then the player keeps its lives
 */
TEST_F(LifeKeeperFixture, StopsListeningOnceDestroyed) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  destroyTheKeeper();

  touchABydo(player);

  EXPECT_EQ(player.lives, THREE_LIVES);
}

}  // namespace rtype::server::testing
