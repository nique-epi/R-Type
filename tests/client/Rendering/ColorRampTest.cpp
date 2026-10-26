#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <span>
#include "ColorRamp.hpp"
#include "ColorRampTestConstants.hpp"
#include "FlashPreference.hpp"

using rtype::client::colorAt;
using rtype::client::FlashPreference;
using rtype::client::rampFor;
using rtype::client::testing::BLUE_SHADE;
using rtype::client::testing::GREEN_SHADE;
using rtype::client::testing::HALF_WAY_THROUGH_A_LIFE;
using rtype::client::testing::PAST_THE_END_OF_A_LIFE;
using rtype::client::testing::RAMP_OF_THREE;
using rtype::client::testing::RED_SHADE;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a ramp of red, green and blue
 * When the color of a life just started is read
 * Then it is the first color, red
 */
TEST(ColorRamp, ALifeStartsOnTheFirstColor) {
  EXPECT_EQ(colorAt(RAMP_OF_THREE, 0.0F), RED_SHADE);
}

/**
 * Given a ramp of red, green and blue
 * When the color half way through a life is read
 * Then it is the middle color, green
 */
TEST(ColorRamp, HalfWayThroughALifeShowsTheMiddleColor) {
  EXPECT_EQ(colorAt(RAMP_OF_THREE, HALF_WAY_THROUGH_A_LIFE), GREEN_SHADE);
}

/**
 * Given a ramp of red, green and blue
 * When the color past the end of a life is read
 * Then it stays on the last color, blue
 */
TEST(ColorRamp, PastItsEndALifeKeepsTheLastColor) {
  EXPECT_EQ(colorAt(RAMP_OF_THREE, PAST_THE_END_OF_A_LIFE), BLUE_SHADE);
}

/**
 * Given a ramp of red, green and blue
 * When it is shown with flashes
 * Then it keeps its three colors
 */
TEST(ColorRamp, WithFlashesARampKeepsItsFirstColor) {
  EXPECT_THAT(rampFor(RAMP_OF_THREE, FlashPreference::Full),
              ElementsAre(RED_SHADE, GREEN_SHADE, BLUE_SHADE));
}

/**
 * Given a ramp of red, green and blue
 * When it is shown with REDUCE FLASHES
 * Then it loses its first color, its flash
 */
TEST(ColorRamp, WithReducedFlashesARampLosesItsFirstColor) {
  EXPECT_THAT(rampFor(RAMP_OF_THREE, FlashPreference::Reduced),
              ElementsAre(GREEN_SHADE, BLUE_SHADE));
}

/**
 * Given an empty ramp
 * When it is shown with REDUCE FLASHES
 * Then it stays empty
 */
TEST(ColorRamp, AnEmptyRampStaysEmpty) {
  EXPECT_THAT(rampFor(std::span<const sf::Color>(), FlashPreference::Reduced),
              IsEmpty());
}
