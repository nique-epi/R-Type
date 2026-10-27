#include <gtest/gtest.h>
#include <cstddef>
#include <random>
#include "BackgroundTestConstants.hpp"
#include "Comet.hpp"
#include "CometTestConstants.hpp"
#include "RecordingPixelSurface.hpp"
#include "SkyEventConstants.hpp"
#include "SkyEventFixtures.hpp"
#include "SkyEventTestConstants.hpp"
#include "SkyLight.hpp"

using rtype::client::Comet;
using rtype::client::COMET_FASTEST_SPEED;
using rtype::client::COMET_HEAD_OFFSETS;
using rtype::client::COMET_SLOWEST_SPEED;
using rtype::client::SkyLight;

/**
 * Given a comet in the sky
 * When it is painted
 * Then every pixel of its tail lies right of its head: the tail trails behind
 */
TEST(Comet, TailTrailsBehindTheHead) {
  std::mt19937 random = generatorFrom(SEED);
  Comet comet(random);
  advanceFor(comet, ONE_EVENT_SECOND);

  const RecordingPixelSurface surface = paintOf(comet, SkyLight::Twinkling);

  ASSERT_GT(surface.plots().size(), COMET_HEAD_OFFSETS.size());
  const std::size_t tailCount =
      surface.plots().size() - COMET_HEAD_OFFSETS.size();
  const int headColumn = surface.plots()[tailCount].position.x;
  for (std::size_t i = 0; i < tailCount; ++i) {
    EXPECT_GE(surface.plots()[i].position.x, headColumn);
  }
}

/**
 * Given a comet in a steady sky, as for a player who reduced flashes
 * When it flies frame by frame for a second, painted at every frame
 * Then it paints its whole tail out of the gaps, and its head, at every
 * frame: no pixel of its tail blinks
 */
TEST(Comet, InASteadySkyItsTailDoesNotFlicker) {
  std::mt19937 random = generatorFrom(SEED);
  Comet comet(random);

  for (int frame = 0; frame < FRAMES_OF_A_SECOND; ++frame) {
    comet.advance(FRAME);
    EXPECT_EQ(paintOf(comet, SkyLight::Steady).plots().size(),
              PLOTS_OF_A_STEADY_COMET)
        << "frame " << frame;
  }
}

/**
 * Given a comet in a twinkling sky
 * When it flies frame by frame for a second, painted at every frame
 * Then the number of pixels it paints changes: its tail flickers
 */
TEST(Comet, InATwinklingSkyItsTailFlickers) {
  std::mt19937 random = generatorFrom(SEED);
  Comet comet(random);
  comet.advance(FRAME);
  const std::size_t firstCount =
      paintOf(comet, SkyLight::Twinkling).plots().size();
  bool changed = false;

  for (int frame = 0; frame < FRAMES_OF_A_SECOND; ++frame) {
    comet.advance(FRAME);
    changed = changed ||
              paintOf(comet, SkyLight::Twinkling).plots().size() != firstCount;
  }

  EXPECT_TRUE(changed);
}

/**
 * Given a new comet
 * When less time passes than its fastest crossing of the sky takes
 * Then it is not over
 */
TEST(Comet, StaysUntilItHasCrossedTheSky) {
  std::mt19937 random = generatorFrom(SEED);
  Comet comet(random);

  advanceFor(comet, (COMET_CROSSING / COMET_FASTEST_SPEED) - FRAME);

  EXPECT_FALSE(comet.isOver());
}

/**
 * Given a new comet
 * When its slowest crossing of the sky has had time to end
 * Then it is over
 */
TEST(Comet, EndsOnceItHasCrossedTheSky) {
  std::mt19937 random = generatorFrom(SEED);
  Comet comet(random);

  advanceFor(comet, (COMET_CROSSING / COMET_SLOWEST_SPEED) + FRAME);

  EXPECT_TRUE(comet.isOver());
}
