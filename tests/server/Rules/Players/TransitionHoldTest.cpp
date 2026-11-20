#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "Intent.hpp"
#include "Invulnerable.hpp"
#include "PlayerController.hpp"
#include "PlayerControllerFixture.hpp"
#include "PlayerControllerTestConstants.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipMotion.hpp"
#include "ShipStartPosition.hpp"
#include "TransitionHold.hpp"
#include "TransitionHoldTestConstants.hpp"

using rtype::server::ConnectedPlayer;
using rtype::server::holdEveryShip;
using rtype::server::returnEveryShip;
using rtype::server::steerShipByInput;
using rtype::server::testing::ANY_INPUT_NUMBER;
using rtype::server::testing::DRIFTING_SPEED;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::HOLDS_SHOOT;
using rtype::server::testing::HOLDS_UP;
using rtype::server::testing::PlayerControllerFixture;
using rtype::server::testing::SESSION_TOKEN;
using rtype::server::testing::TICKS_LEFT_OF_THE_TRANSITION;
using rtype::server::testing::WHERE_THE_SHIP_FLEW;

/**
 * Given a player holding up and the shot when a transition of the level holds
 * its ship
 * When the ship is held
 * Then it wants nothing: it neither moves nor fires
 */
TEST_F(PlayerControllerFixture, AHeldShipWantsNothing) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);
  steerShipByInput(world(), player,
                   inputHolding(HOLDS_UP | HOLDS_SHOOT, ANY_INPUT_NUMBER));

  holdEveryShip(world(), roster(), TICKS_LEFT_OF_THE_TRANSITION);

  const auto* const intent =
      world().shipComponentOf<rtype::game::Intent>(player.shipEntityId);
  ASSERT_NE(intent, nullptr);
  EXPECT_EQ(intent->direction.y, 0.0F);
  EXPECT_FALSE(intent->shooting);
}

/**
 * Given a ship drifting to the right
 * When a transition of the level holds it
 * Then it is at rest: nothing is left to carry it
 */
TEST_F(PlayerControllerFixture, AHeldShipStopsDrifting) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);
  auto* const motion =
      world().shipComponentOf<rtype::game::ShipMotion>(player.shipEntityId);
  ASSERT_NE(motion, nullptr);
  motion->velocityX = DRIFTING_SPEED;

  holdEveryShip(world(), roster(), TICKS_LEFT_OF_THE_TRANSITION);

  EXPECT_EQ(motion->velocityX, 0.0F);
}

/**
 * Given a ship that is not invulnerable
 * When a transition of the level with 120 ticks left holds it
 * Then it stays invulnerable for those 120 ticks
 */
TEST_F(PlayerControllerFixture, AHeldShipIsInvulnerableUntilTheEnd) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);

  holdEveryShip(world(), roster(), TICKS_LEFT_OF_THE_TRANSITION);

  const auto* const protection =
      world().shipComponentOf<rtype::game::Invulnerable>(player.shipEntityId);
  ASSERT_NE(protection, nullptr);
  EXPECT_EQ(protection->ticksLeft, TICKS_LEFT_OF_THE_TRANSITION);
}

/**
 * Given a ship far from where the ships start
 * When the ships come back at the end of a transition
 * Then it is where a new ship of its player appears
 */
TEST_F(PlayerControllerFixture, AReturnedShipIsAtItsStart) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);
  auto* const position =
      world().shipComponentOf<rtype::game::Position>(player.shipEntityId);
  ASSERT_NE(position, nullptr);
  *position = WHERE_THE_SHIP_FLEW;

  returnEveryShip(world(), roster());

  const rtype::game::Position start = rtype::game::shipStartPosition(
      player.playerNumber, rtype::network::MAXIMUM_PLAYERS);
  EXPECT_FLOAT_EQ(position->x, start.x);
  EXPECT_FLOAT_EQ(position->y, start.y);
}
