#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include "IntroCastConstants.hpp"
#include "IntroShipPaths.hpp"
#include "IntroTestConstants.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"

using rtype::client::closeupPlaceAt;
using rtype::client::closeupStartOf;
using rtype::client::exhaustAt;
using rtype::client::INTRO_BEAT;
using rtype::client::INTRO_GREEN_PILOT;
using rtype::client::INTRO_HOMES;
using rtype::client::INTRO_PINK_PILOT;
using rtype::client::INTRO_SCENE_BOOST;
using rtype::client::INTRO_SCENE_CHARGE;
using rtype::client::INTRO_SWARM_CUT;
using rtype::client::squadronPlaceAt;
using rtype::client::testing::A_MOMENT;
using rtype::client::testing::HALF_A_BEAT;
using rtype::client::testing::MIDDLE_OF_A_CLOSEUP_X;
using rtype::client::testing::ONE_SECOND;
using rtype::client::testing::SCREEN_WIDTH;
using rtype::client::testing::SQUADRON_ENTRY_X;
using rtype::client::testing::SWAPPED_ROWS_GAP;
using rtype::client::testing::TWO_BOBS;

/**
 * Given the close-up of the second pilot
 * When the place of its ship is read just before its beat and at its beat
 * Then the ship shows only from its beat
 */
TEST(IntroShipPaths, APilotShowsFromTheBeatOfItsCloseUp) {
  EXPECT_FALSE(closeupPlaceAt(1, closeupStartOf(1) - A_MOMENT).has_value());
  EXPECT_TRUE(closeupPlaceAt(1, closeupStartOf(1)).has_value());
}

/**
 * Given the close-up of the second pilot
 * When the place of its ship is read once its beat is over
 * Then the ship is gone
 */
TEST(IntroShipPaths, APilotLeavesWithTheEndOfItsBeat) {
  EXPECT_FALSE(closeupPlaceAt(1, closeupStartOf(1) + INTRO_BEAT).has_value());
}

/**
 * Given the close-up of the first pilot
 * When the place of its ship is read in the middle of its beat
 * Then the ship is centered on the shot
 */
TEST(IntroShipPaths, AShipIsCenteredInTheMiddleOfItsCloseUp) {
  const std::optional<sf::Vector2f> place =
      closeupPlaceAt(0, closeupStartOf(0) + HALF_A_BEAT);

  ASSERT_TRUE(place.has_value());
  EXPECT_FLOAT_EQ(place.value_or(sf::Vector2f()).x, MIDDLE_OF_A_CLOSEUP_X);
}

/**
 * Given the cut to the swarm
 * When the place of the first ship is read at the cut
 * Then it enters from 70 pixels left of the screen
 */
TEST(IntroShipPaths, TheSquadronEntersFromTheLeft) {
  const std::optional<sf::Vector2f> place = squadronPlaceAt(0, INTRO_SWARM_CUT);

  ASSERT_TRUE(place.has_value());
  EXPECT_FLOAT_EQ(place.value_or(sf::Vector2f()).x, SQUADRON_ENTRY_X);
}

/**
 * Given the swap of the pink and green ships, over before the charge
 * When their places are read just before the charge
 * Then the pink ship flies the row of the green one, 48 pixels lower, within
 * a bob of each
 */
TEST(IntroShipPaths, ThePinkAndGreenShipsSwapRows) {
  const float scene = INTRO_SCENE_CHARGE - A_MOMENT;
  const std::optional<sf::Vector2f> pink =
      squadronPlaceAt(INTRO_PINK_PILOT, scene);
  const std::optional<sf::Vector2f> green =
      squadronPlaceAt(INTRO_GREEN_PILOT, scene);

  ASSERT_TRUE(pink.has_value() && green.has_value());
  EXPECT_NEAR(
      pink.value_or(sf::Vector2f()).y - green.value_or(sf::Vector2f()).y,
      SWAPPED_ROWS_GAP, TWO_BOBS);
}

/**
 * Given the boost
 * When the places of the ships are read one second later
 * Then every ship left past the right edge
 */
TEST(IntroShipPaths, TheShipsLeaveRightAtTheBoost) {
  for (std::size_t pilot = 0; pilot < INTRO_HOMES.size(); ++pilot) {
    const std::optional<sf::Vector2f> place =
        squadronPlaceAt(pilot, INTRO_SCENE_BOOST + ONE_SECOND);
    ASSERT_TRUE(place.has_value());
    EXPECT_GT(place.value_or(sf::Vector2f()).x, SCREEN_WIDTH) << pilot;
  }
}

/**
 * Given the boost
 * When the length of the flames is read
 * Then they burn full
 */
TEST(IntroShipPaths, TheFlamesBurnFullAtTheBoost) {
  EXPECT_FLOAT_EQ(exhaustAt(INTRO_SCENE_BOOST), 1.0F);
}
