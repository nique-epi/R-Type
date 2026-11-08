#include <gtest/gtest.h>
#include <array>
#include "Entity.hpp"
#include "PilotShipLooksFixture.hpp"
#include "PlayerLooks.hpp"
#include "ShipLook.hpp"

using rtype::engine::Entity;
using rtype::identifiers::ShipLook;
using rtype::network::PlayerLook;

namespace {

/** @brief Pilot colors 0 and 1 belong to players 1 and 2. */
constexpr int FIRST_COLOR = 0;
constexpr int SECOND_COLOR = 1;
constexpr ShipLook TOP_HAT_LOOK{.head = 1};
constexpr ShipLook SHADES_LOOK{.eyes = 9};

}  // namespace

/**
 * Given the ships of players 1 and 2, and looks that list the top hat for the
 * first and the shades for the second
 * When the ships are dressed
 * Then each ship wears the look of its own pilot
 */
TEST_F(PilotShipLooksFixture, AShipWearsTheLookOfItsPilot) {
  const Entity first = spawnShip(FIRST_COLOR);
  const Entity second = spawnShip(SECOND_COLOR);
  const std::array looks{PlayerLook{.playerNumber = 1, .look = TOP_HAT_LOOK},
                         PlayerLook{.playerNumber = 2, .look = SHADES_LOOK}};

  dress(looks);

  EXPECT_EQ(wornBy(first), TOP_HAT_LOOK);
  EXPECT_EQ(wornBy(second), SHADES_LOOK);
}

/**
 * Given the ship of player 2 and looks that list player 1 only
 * When the ship is dressed
 * Then it wears nothing
 */
TEST_F(PilotShipLooksFixture, AShipWhosePilotIsNotListedWearsNothing) {
  const Entity second = spawnShip(SECOND_COLOR);
  const std::array looks{PlayerLook{.playerNumber = 1, .look = TOP_HAT_LOOK}};

  dress(looks);

  EXPECT_EQ(wornBy(second), ShipLook{});
}

/**
 * Given the ship of player 1 dressed with the top hat
 * When the looks list the shades for it and it is dressed again
 * Then it wears the shades alone
 */
TEST_F(PilotShipLooksFixture, ALookThatChangesIsFollowed) {
  const Entity first = spawnShip(FIRST_COLOR);
  const std::array before{PlayerLook{.playerNumber = 1, .look = TOP_HAT_LOOK}};
  dress(before);
  const std::array after{PlayerLook{.playerNumber = 1, .look = SHADES_LOOK}};

  dress(after);

  EXPECT_EQ(wornBy(first), SHADES_LOOK);
}
