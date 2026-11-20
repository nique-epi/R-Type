#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "CasinoLighting.hpp"
#include "CasinoLightingTestConstants.hpp"
#include "ColoredPixel.hpp"
#include "LightBeam.hpp"

using rtype::client::addBeamPixels;
using rtype::client::chaseStepAt;
using rtype::client::ColoredPixel;
using rtype::client::isLitBulb;
using rtype::client::LightBeam;
using rtype::client::testing::BEAM_COLOR;
using rtype::client::testing::BEAM_HALF_SPREAD;
using rtype::client::testing::BEAM_LENGTH;
using rtype::client::testing::BEAM_ORIGIN;
using rtype::client::testing::BESIDE_THE_BEAM;
using rtype::client::testing::BLUE_AT_THE_FLOOR;
using rtype::client::testing::FRAME_GOLD;
using rtype::client::testing::GREEN_AT_THE_FLOOR;
using rtype::client::testing::JUST_SHORT_OF_A_STEP;
using rtype::client::testing::NEAR_THE_TOP_EDGE;
using rtype::client::testing::ON_THE_BEAM_AXIS;
using rtype::client::testing::ONE_WHOLE_STEP;
using rtype::client::testing::PALE_CREAM_BULB;
using rtype::client::testing::RED_AT_THE_FLOOR;
using rtype::client::testing::STRAIGHT_UP;
using testing::Contains;
using testing::Each;
using testing::Field;
using testing::Ge;
using testing::IsEmpty;
using testing::Not;

/**
 * Given the pale cream of a bulb of the pictures of the casino
 * When it is read
 * Then it is a lit bulb
 */
TEST(CasinoLighting, APaleCreamPixelIsALitBulb) {
  EXPECT_TRUE(isLitBulb(PALE_CREAM_BULB));
}

/**
 * Given a pale pixel with one channel at the floor a lit bulb passes
 * When each is read
 * Then none is a lit bulb, so the floors are strict
 */
TEST(CasinoLighting, AChannelAtItsFloorIsNoBulb) {
  EXPECT_FALSE(isLitBulb(RED_AT_THE_FLOOR));
  EXPECT_FALSE(isLitBulb(GREEN_AT_THE_FLOOR));
  EXPECT_FALSE(isLitBulb(BLUE_AT_THE_FLOOR));
}

/**
 * Given the gold of the frame around the bulbs
 * When it is read
 * Then it is no bulb
 */
TEST(CasinoLighting, TheGoldOfAFrameIsNoBulb) {
  EXPECT_FALSE(isLitBulb(FRAME_GOLD));
}

/**
 * Given a chase that began just short of one step ago, then one whole step
 * When its step is read
 * Then it is still the first, then the second
 */
TEST(CasinoLighting, TheChaseMovesOnceAWholeStepPassed) {
  EXPECT_EQ(chaseStepAt(JUST_SHORT_OF_A_STEP), 0);
  EXPECT_EQ(chaseStepAt(ONE_WHOLE_STEP), 1);
}

/**
 * Given a thin beam pointing straight up
 * When its pixels are added
 * Then the pixel on its axis is lit, in the color of the beam
 */
TEST(CasinoLighting, ABeamLightsThePixelsOnItsAxis) {
  std::vector<ColoredPixel> pixels;

  addBeamPixels(pixels, LightBeam{.origin = BEAM_ORIGIN,
                                  .angle = STRAIGHT_UP,
                                  .length = BEAM_LENGTH,
                                  .halfSpread = BEAM_HALF_SPREAD,
                                  .color = BEAM_COLOR});

  EXPECT_THAT(pixels,
              Contains(Field(&ColoredPixel::position, ON_THE_BEAM_AXIS)));
  EXPECT_THAT(pixels, Each(Field(&ColoredPixel::color, BEAM_COLOR)));
}

/**
 * Given a thin beam pointing straight up
 * When its pixels are added
 * Then a pixel four columns aside of its axis stays dark
 */
TEST(CasinoLighting, ABeamLeavesThePixelsAsideDark) {
  std::vector<ColoredPixel> pixels;

  addBeamPixels(pixels, LightBeam{.origin = BEAM_ORIGIN,
                                  .angle = STRAIGHT_UP,
                                  .length = BEAM_LENGTH,
                                  .halfSpread = BEAM_HALF_SPREAD,
                                  .color = BEAM_COLOR});

  EXPECT_THAT(pixels,
              Not(Contains(Field(&ColoredPixel::position, BESIDE_THE_BEAM))));
}

/**
 * Given a beam pointing up out of the top of the interface
 * When its pixels are added
 * Then none lies above the interface, whose first row is 0
 */
TEST(CasinoLighting, ABeamIsCutAtTheEdgeOfTheInterface) {
  std::vector<ColoredPixel> pixels;

  addBeamPixels(pixels, LightBeam{.origin = NEAR_THE_TOP_EDGE,
                                  .angle = STRAIGHT_UP,
                                  .length = BEAM_LENGTH,
                                  .halfSpread = BEAM_HALF_SPREAD,
                                  .color = BEAM_COLOR});

  EXPECT_THAT(pixels, Not(IsEmpty()));
  EXPECT_THAT(pixels, Each(Field(&ColoredPixel::position,
                                 Field(&sf::Vector2i::y, Ge(0)))));
}
