#include <gtest/gtest.h>
#include <array>
#include "ConnectedPlayer.hpp"
#include "GameInstanceTestConstants.hpp"
#include "Intent.hpp"
#include "ManeuverKind.hpp"
#include "PlayerController.hpp"
#include "PlayerControllerFixture.hpp"
#include "PlayerControllerTestConstants.hpp"
#include "PlayerInput.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipInput.hpp"
#include "ShipMotion.hpp"

namespace rtype::server::testing {
namespace {

/**
 * Given a player with a ship that wants nothing
 * When an input holding up and shoot arrives
 * Then the Intent of the ship holds up and shoot, and nothing else
 */
TEST_F(PlayerControllerFixture, TurnsAnInputIntoTheIntentOfTheShip) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);

  steerShipByInput(world(), player,
                   inputHolding(HOLDS_UP | HOLDS_SHOOT, ANY_INPUT_NUMBER));

  const game::Intent* const intent =
      world().shipComponentOf<game::Intent>(player.shipEntityId);
  ASSERT_NE(intent, nullptr);
  EXPECT_TRUE(intent->directions.up);
  EXPECT_FALSE(intent->directions.down);
  EXPECT_FALSE(intent->directions.left);
  EXPECT_FALSE(intent->directions.right);
  EXPECT_TRUE(intent->shooting);
}

/**
 * Given a ship whose Intent holds up
 * When an input holding nothing arrives
 * Then the Intent of the ship holds nothing: the newest input replaces the
 * last one
 */
TEST_F(PlayerControllerFixture, ReplacesTheIntentWithTheNewestInput) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);
  steerShipByInput(world(), player, inputHolding(HOLDS_UP, ANY_INPUT_NUMBER));

  steerShipByInput(world(), player,
                   inputHolding(HOLDS_NOTHING, ANY_INPUT_NUMBER + 1));

  const game::Intent* const intent =
      world().shipComponentOf<game::Intent>(player.shipEntityId);
  ASSERT_NE(intent, nullptr);
  EXPECT_FALSE(intent->directions.up);
}

/**
 * Given a player without a ship
 * When an input arrives
 * Then nothing happens: no ship is steered and nothing throws
 */
TEST_F(PlayerControllerFixture, SteersNothingForAPlayerWithoutAShip) {
  const ConnectedPlayer& player = addPlayer(FIRST_PLAYER, SESSION_TOKEN);

  steerShipByInput(world(), player, inputHolding(HOLDS_UP, ANY_INPUT_NUMBER));

  EXPECT_EQ(world().entityCount(), 0U);
}

/**
 * Given two ships whose Intent holds up and shoot, the player of the second one
 * away
 * When the away ships are silenced
 * Then the Intent of the second ship holds nothing and the first is unchanged
 */
TEST_F(PlayerControllerFixture, SilencesTheShipOfAnAwayPlayerOnly) {
  const ConnectedPlayer& present =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);
  ConnectedPlayer& away = addPlayerWithShip(SECOND_PLAYER, OTHER_SESSION_TOKEN);
  const network::PlayerInput held =
      inputHolding(HOLDS_UP | HOLDS_SHOOT, ANY_INPUT_NUMBER);
  steerShipByInput(world(), present, held);
  steerShipByInput(world(), away, held);
  away.away = true;

  silenceAwayShips(world(), roster());

  const game::Intent* const presentIntent =
      world().shipComponentOf<game::Intent>(present.shipEntityId);
  const game::Intent* const awayIntent =
      world().shipComponentOf<game::Intent>(away.shipEntityId);
  ASSERT_NE(presentIntent, nullptr);
  ASSERT_NE(awayIntent, nullptr);
  EXPECT_TRUE(presentIntent->directions.up);
  EXPECT_TRUE(presentIntent->shooting);
  EXPECT_FALSE(awayIntent->directions.up);
  EXPECT_FALSE(awayIntent->shooting);
}

/**
 * Given a player who sent inputs to an earlier ship and was holding right
 * When its new ship gets its first input (holding right), a release, and a
 * second press within DOUBLE_TAP_INPUTS of the first
 * Then no dash starts: the first input of a new ship presses nothing
 */
TEST_F(PlayerControllerFixture, FirstInputOfANewShipStartsNoManeuver) {
  ConnectedPlayer& player = addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);
  player.inputApplied = true;
  const std::array inputs = {
      inputHolding(HOLDS_RIGHT, FIRST_PRESS_INPUT_NUMBER),
      inputHolding(HOLDS_NOTHING, RELEASE_INPUT_NUMBER),
      inputHolding(HOLDS_RIGHT, SECOND_PRESS_INPUT_NUMBER)};

  for (const network::PlayerInput& input : inputs) {
    noteTaps(world(), player, input);
    steerShipByInput(world(), player, input);
  }

  const game::ShipMotion* const motion =
      world().shipMotionOf(player.shipEntityId);
  ASSERT_NE(motion, nullptr);
  EXPECT_EQ(motion->maneuver, game::ManeuverKind::None);
}

/**
 * Given a player who sent no input yet, whose first input holds right
 * When that press, a release and a second press within DOUBLE_TAP_INPUTS of the
 * first arrive
 * Then a dash starts: the first input of a session still counts as a press
 */
TEST_F(PlayerControllerFixture, FirstInputOfASessionStillPressesWhatItHolds) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);
  const std::array inputs = {
      inputHolding(HOLDS_RIGHT, FIRST_PRESS_INPUT_NUMBER),
      inputHolding(HOLDS_NOTHING, RELEASE_INPUT_NUMBER),
      inputHolding(HOLDS_RIGHT, SECOND_PRESS_INPUT_NUMBER)};

  for (const network::PlayerInput& input : inputs) {
    noteTaps(world(), player, input);
    steerShipByInput(world(), player, input);
  }

  const game::ShipMotion* const motion =
      world().shipComponentOf<game::ShipMotion>(player.shipEntityId);
  ASSERT_NE(motion, nullptr);
  EXPECT_EQ(motion->maneuver, game::ManeuverKind::Dash);
}

/**
 * Given a ship that already received an input releasing right
 * When a press, a release and a second press within DOUBLE_TAP_INPUTS of the
 * first press arrive
 * Then a dash starts: the double press of a ship that was steered still counts
 */
TEST_F(PlayerControllerFixture, SecondPressOfASteeredShipStartsADash) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);
  const std::array inputs = {
      inputHolding(HOLDS_NOTHING, FIRST_PRESS_INPUT_NUMBER - 1),
      inputHolding(HOLDS_RIGHT, FIRST_PRESS_INPUT_NUMBER),
      inputHolding(HOLDS_NOTHING, RELEASE_INPUT_NUMBER),
      inputHolding(HOLDS_RIGHT, SECOND_PRESS_INPUT_NUMBER)};

  for (const network::PlayerInput& input : inputs) {
    noteTaps(world(), player, input);
    steerShipByInput(world(), player, input);
  }

  const game::ShipMotion* const motion =
      world().shipMotionOf(player.shipEntityId);
  ASSERT_NE(motion, nullptr);
  EXPECT_EQ(motion->maneuver, game::ManeuverKind::Dash);
}

}  // namespace
}  // namespace rtype::server::testing
