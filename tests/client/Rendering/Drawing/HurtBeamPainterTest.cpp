#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "ColoredPixel.hpp"
#include "CurrentAction.hpp"
#include "HurtBeamPainterConstants.hpp"
#include "HurtBeamPainterFixtures.hpp"
#include "HurtBeamPainterTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::BEAM_DEEP_MAGENTA;
using rtype::client::BEAM_WHITE;
using rtype::client::ColoredPixel;
using rtype::client::testing::BEAM_STRIKE;
using rtype::client::testing::colorAt;
using rtype::client::testing::HALF_THE_SWEEP;
using rtype::client::testing::ON_THE_BEAM_AXIS;
using rtype::client::testing::ON_THE_BEAM_RIM;
using rtype::client::testing::ON_THE_SWEEP_HALFWAY;
using rtype::client::testing::paintedPixels;
using rtype::client::testing::PAST_THE_BEAM_RIM;
using rtype::client::testing::SWEEP_STRIKE;
using rtype::engine::Duration;
using rtype::game::CurrentAction;

/**
 * Given the Bydo Tyrant at rest, showing the strike of its beam
 * When its hurt beams are painted
 * Then the pixel 2 units off the line of the beam is its white core
 */
TEST(HurtBeamPainter, PaintsTheCoreOfAStraightBeamOnItsAxis) {
  const std::vector<ColoredPixel> pixels = paintedPixels(
      CurrentAction{.key = BEAM_STRIKE, .since = Duration::zero()});

  EXPECT_EQ(colorAt(pixels, ON_THE_BEAM_AXIS), std::optional{BEAM_WHITE});
}

/**
 * Given the Bydo Tyrant at rest, showing the strike of its beam, 52 thick
 * When its hurt beams are painted
 * Then the pixel 26 units off its line is its rim, and the one 30 units off
 * is left as it is: the band is as thick as the beam the server hurts with
 */
TEST(HurtBeamPainter, PaintsTheBandAsThickAsTheBeam) {
  const std::vector<ColoredPixel> pixels = paintedPixels(
      CurrentAction{.key = BEAM_STRIKE, .since = Duration::zero()});

  EXPECT_EQ(colorAt(pixels, ON_THE_BEAM_RIM), std::optional{BEAM_DEEP_MAGENTA});
  EXPECT_EQ(colorAt(pixels, PAST_THE_BEAM_RIM), std::nullopt);
}

/**
 * Given the Bydo Tyrant at rest, halfway through the strike of its sweep
 * When its hurt beams are painted
 * Then the pixel on its line at -11.5 degrees is its white core, where the
 * server lays it then
 */
TEST(HurtBeamPainter, PaintsASweepAtTheAngleOfItsTime) {
  const std::vector<ColoredPixel> pixels = paintedPixels(
      CurrentAction{.key = SWEEP_STRIKE, .since = HALF_THE_SWEEP});

  EXPECT_EQ(colorAt(pixels, ON_THE_SWEEP_HALFWAY), std::optional{BEAM_WHITE});
}

/**
 * Given the Bydo Tyrant at rest, as the strike of its sweep starts
 * When its hurt beams are painted
 * Then the pixel its sweep reaches halfway through is left as it is: the
 * beam starts at -35 degrees
 */
TEST(HurtBeamPainter, PaintsASweepWhereItStarts) {
  const std::vector<ColoredPixel> pixels = paintedPixels(
      CurrentAction{.key = SWEEP_STRIKE, .since = Duration::zero()});

  EXPECT_EQ(colorAt(pixels, ON_THE_SWEEP_HALFWAY), std::nullopt);
}

/**
 * Given the Bydo Tyrant at rest, idle
 * When its hurt beams are painted
 * Then nothing is painted: its beams hurt only in their strike
 */
TEST(HurtBeamPainter, PaintsNothingWhileIdle) {
  const std::vector<ColoredPixel> pixels = paintedPixels(CurrentAction{});

  EXPECT_TRUE(pixels.empty());
}
