#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "PlayBand.hpp"
#include "PlayerControllerFixture.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipBand.hpp"
#include "ShipBandTestConstants.hpp"

using rtype::game::PlayBand;
using rtype::server::bandEveryShip;
using rtype::server::ConnectedPlayer;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::PIT_BAND;
using rtype::server::testing::PlayerControllerFixture;
using rtype::server::testing::SESSION_TOKEN;

/**
 * Given a new ship
 * When nothing gives it a band
 * Then it flies in the whole play area
 */
TEST_F(PlayerControllerFixture, ANewShipFliesInTheWholePlayArea) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);

  const auto* const band =
      world().shipComponentOf<PlayBand>(player.shipEntityId);

  ASSERT_NE(band, nullptr);
  EXPECT_EQ(*band, PlayBand{});
}

/**
 * Given a ship
 * When the band of the Pit is given to every ship
 * Then the ship flies between the ceiling and the floor of the Pit
 */
TEST_F(PlayerControllerFixture, EveryShipTakesTheBandOfTheBiome) {
  const ConnectedPlayer& player =
      addPlayerWithShip(FIRST_PLAYER, SESSION_TOKEN);

  bandEveryShip(world(), roster(), PIT_BAND);

  const auto* const band =
      world().shipComponentOf<PlayBand>(player.shipEntityId);
  ASSERT_NE(band, nullptr);
  EXPECT_EQ(*band, PIT_BAND);
}
