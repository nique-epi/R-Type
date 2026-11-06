#include <gtest/gtest.h>
#include <algorithm>
#include <numbers>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "ColoredPixel.hpp"
#include "InterfaceColorConstants.hpp"
#include "MotionPreference.hpp"
#include "PlanetPose.hpp"
#include "RingedPlanet.hpp"
#include "RingedPlanetFixtures.hpp"
#include "RingedPlanetTestConstants.hpp"

using rtype::client::BLUE;
using rtype::client::ColoredPixel;
using rtype::client::DEEP_BLUE;
using rtype::client::HUD_CYAN;
using rtype::client::MotionPreference;
using rtype::client::PALE_BLUE;
using rtype::client::PlanetPose;
using rtype::client::ringedPlanetPixels;
using rtype::client::turnedPlanet;
using rtype::client::testing::A_WHOLE_TURN;
using rtype::client::testing::countOf;
using rtype::client::testing::HALF_A_TURN;
using rtype::client::testing::INSIDE_THE_FIRST_FRAME;
using rtype::client::testing::MOCKUP_BLUE_AT_REST;
using rtype::client::testing::MOCKUP_CYAN_AT_REST;
using rtype::client::testing::MOCKUP_CYAN_TURNED;
using rtype::client::testing::MOCKUP_DEEP_AT_REST;
using rtype::client::testing::MOCKUP_PALE_AT_REST;
using rtype::client::testing::MOCKUP_PALE_PIXEL;
using rtype::client::testing::MOCKUP_PIXELS_AT_REST;
using rtype::client::testing::MOCKUP_PIXELS_TURNED;
using rtype::client::testing::mockupPlanet;
using rtype::client::testing::RIGHT_HALF;
using rtype::client::testing::TURNED_DRIFT;
using rtype::client::testing::TURNED_SPIN;
using rtype::client::testing::WHOLE_PICTURE;

/**
 * Given the planet of the mockup at rest
 * When its pixels are drawn
 * Then there are as many as the kit of the mockup paints, color by color
 */
TEST(RingedPlanet, PaintsThePixelsOfTheMockupAtRest) {
  const std::vector<ColoredPixel> pixels =
      ringedPlanetPixels(mockupPlanet(0.0, 0.0), WHOLE_PICTURE);

  EXPECT_EQ(pixels.size(), MOCKUP_PIXELS_AT_REST);
  EXPECT_EQ(countOf(pixels, DEEP_BLUE), MOCKUP_DEEP_AT_REST);
  EXPECT_EQ(countOf(pixels, BLUE), MOCKUP_BLUE_AT_REST);
  EXPECT_EQ(countOf(pixels, PALE_BLUE), MOCKUP_PALE_AT_REST);
  EXPECT_EQ(countOf(pixels, HUD_CYAN), MOCKUP_CYAN_AT_REST);
}

/**
 * Given the planet of the mockup turned and its grains drifted
 * When its pixels are drawn
 * Then there are as many as the kit of the mockup paints in that pose
 */
TEST(RingedPlanet, PaintsThePixelsOfTheMockupTurned) {
  const std::vector<ColoredPixel> pixels = ringedPlanetPixels(
      mockupPlanet(TURNED_SPIN, TURNED_DRIFT), WHOLE_PICTURE);

  EXPECT_EQ(pixels.size(), MOCKUP_PIXELS_TURNED);
  EXPECT_EQ(countOf(pixels, HUD_CYAN), MOCKUP_CYAN_TURNED);
}

/**
 * Given the planet of the mockup at rest
 * When its pixels are drawn
 * Then the pixel the mockup paints pale is pale
 */
TEST(RingedPlanet, PaintsAPixelOfTheMockupInItsColor) {
  const std::vector<ColoredPixel> pixels =
      ringedPlanetPixels(mockupPlanet(0.0, 0.0), WHOLE_PICTURE);

  const auto found =
      std::ranges::find(pixels, MOCKUP_PALE_PIXEL, &ColoredPixel::position);
  ASSERT_NE(found, pixels.end());
  EXPECT_EQ(found->color, PALE_BLUE);
}

/**
 * Given the right half of the picture, as the map shows the planet
 * When its pixels are drawn in it
 * Then none lies outside it
 */
TEST(RingedPlanet, PaintsNothingOutsideItsClip) {
  const std::vector<ColoredPixel> pixels =
      ringedPlanetPixels(mockupPlanet(0.0, 0.0), RIGHT_HALF);

  EXPECT_FALSE(pixels.empty());
  EXPECT_TRUE(
      std::ranges::all_of(pixels, [](const ColoredPixel& pixel) noexcept {
        return RIGHT_HALF.contains(pixel.position);
      }));
}

/**
 * Given a planet at rest
 * When 20 ms pass, inside the first frame of 24 a second
 * Then it has not turned yet
 */
TEST(RingedPlanet, TurnsByWholeFramesOnly) {
  const PlanetPose turned = turnedPlanet(
      mockupPlanet(0.0, 0.0), INSIDE_THE_FIRST_FRAME, MotionPreference::Full);

  EXPECT_EQ(turned.spin, 0.0);
}

/**
 * Given a planet at rest
 * When 30 s pass, one whole turn
 * Then it is back where it started
 */
TEST(RingedPlanet, ComesBackAfterAWholeTurn) {
  const PlanetPose turned = turnedPlanet(mockupPlanet(0.0, 0.0), A_WHOLE_TURN,
                                         MotionPreference::Full);

  EXPECT_EQ(turned.spin, 0.0);
}

/**
 * Given a planet at rest
 * When 15 s pass
 * Then it has turned half a turn
 */
TEST(RingedPlanet, TurnsHalfATurnInFifteenSeconds) {
  const PlanetPose turned =
      turnedPlanet(mockupPlanet(0.0, 0.0), HALF_A_TURN, MotionPreference::Full);

  EXPECT_DOUBLE_EQ(turned.spin, std::numbers::pi);
}

/**
 * Given a player who asked for reduced motion
 * When 15 s pass
 * Then the planet has not turned
 */
TEST(RingedPlanet, StaysStillUnderReducedMotion) {
  const PlanetPose turned = turnedPlanet(mockupPlanet(0.0, 0.0), HALF_A_TURN,
                                         MotionPreference::Reduced);

  EXPECT_EQ(turned.spin, 0.0);
}
