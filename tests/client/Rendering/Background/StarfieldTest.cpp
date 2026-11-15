#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "BackgroundTestConstants.hpp"
#include "ColoredPixel.hpp"
#include "EffectTestConstants.hpp"
#include "PlanetImage.hpp"
#include "RecordingPixelSurface.hpp"
#include "Star.hpp"
#include "StarPlane.hpp"
#include "Starfield.hpp"
#include "StarfieldFixtures.hpp"
#include "StarfieldTestConstants.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::BACKGROUND_HEIGHT;
using rtype::client::BACKGROUND_WIDTH;
using rtype::client::BLUE;
using rtype::client::ColoredPixel;
using rtype::client::DEEP_BLUE;
using rtype::client::MAXIMUM_BACKGROUND_STEP;
using rtype::client::PLANET_REENTRY_MARGIN;
using rtype::client::PLANET_SPEED;
using rtype::client::PlanetImage;
using rtype::client::Star;
using rtype::client::STAR_LAYERS;
using rtype::client::STAR_STREAK_COLORS;
using rtype::client::STAR_TWINKLE_CYCLE_STEPS;
using rtype::client::STAR_WRAP_MARGIN;
using rtype::client::StarDepth;
using rtype::client::Starfield;
using rtype::client::StarLayer;
using rtype::client::StarPlane;
using rtype::engine::Duration;
using rtype::engine::testing::ONE_SECOND;

/**
 * Given a new starfield
 * When its stars are counted by depth
 * Then each layer holds the number of stars it is meant to
 */
TEST(Starfield, EveryLayerHoldsItsStars) {
  const Starfield starfield(SEED);

  for (const StarLayer& layer : STAR_LAYERS) {
    EXPECT_EQ(countAt(starfield, layer.depth), layer.count);
  }
}

/**
 * Given a new starfield
 * When its stars are located
 * Then every star lies inside the background
 */
TEST(Starfield, StarsStartInsideTheBackground) {
  const Starfield starfield(SEED);

  for (const Star& star : starfield.stars()) {
    EXPECT_GE(starfield.positionOf(star), 0.0F);
    EXPECT_LE(starfield.positionOf(star), BACKGROUND_WIDTH);
    EXPECT_GE(star.height, 0.0F);
    EXPECT_LE(star.height, BACKGROUND_HEIGHT);
  }
}

/**
 * Given a starfield
 * When ten milliseconds pass
 * Then every star has moved left by the speed of its layer for that time, at
 * the same height
 */
TEST(Starfield, StarsMoveLeftAtTheSpeedOfTheirLayer) {
  Starfield starfield(SEED);
  const std::vector<Star> before = starfield.stars();
  const std::vector<float> positionsBefore = positionsOf(starfield);

  starfield.advance(SHORT_STEP);

  for (std::size_t i = 0; i < before.size(); ++i) {
    const Star& star = starfield.stars()[i];
    EXPECT_NEAR(
        starfield.positionOf(star),
        positionsBefore[i] - (speedOf(star.depth) * seconds(SHORT_STEP)),
        POSITION_TOLERANCE);
    EXPECT_EQ(star.height, before[i].height);
  }
}

/**
 * Given two starfields with the same seed
 * When one second passes in frames of the maximum step for one, and in frames
 * half as long for the other
 * Then their stars are at the same columns: movement follows time, not frames
 */
TEST(Starfield, MovementFollowsTimeNotFrames) {
  Starfield fewFrames(SEED);
  Starfield manyFrames(SEED);

  advanceBy(fewFrames, ONE_SECOND);
  for (Duration left = ONE_SECOND; left > Duration::zero();
       left -= HALF_MAXIMUM_STEP) {
    manyFrames.advance(HALF_MAXIMUM_STEP);
  }

  expectSameColumns(fewFrames, manyFrames);
}

/**
 * Given two starfields with the same seed
 * When one gets a frame of one second and the other a frame of the maximum
 * step
 * Then their stars are at the same columns: a stall moves the sky no further
 * than the maximum step
 */
TEST(Starfield, StallMovesNoFurtherThanTheMaximumStep) {
  Starfield stalled(SEED);
  Starfield regular(SEED);

  stalled.advance(ONE_SECOND);
  regular.advance(MAXIMUM_BACKGROUND_STEP);

  expectSameColumns(stalled, regular);
}

/**
 * Given a starfield
 * When a frame of zero time passes
 * Then no star moves
 */
TEST(Starfield, ZeroTimeMovesNothing) {
  Starfield starfield(SEED);
  const Starfield untouched(SEED);

  starfield.advance(Duration::zero());

  expectSameColumns(starfield, untouched);
}

/**
 * Given a starfield
 * When a frame of negative time passes
 * Then no star moves
 */
TEST(Starfield, NegativeTimeMovesNothing) {
  Starfield starfield(SEED);
  const Starfield untouched(SEED);

  starfield.advance(NEGATIVE_STEP);

  expectSameColumns(starfield, untouched);
}

/**
 * Given a starfield
 * When a minute passes, frame by frame
 * Then no star is ever painted outside the band between the two wrap margins:
 * stars leaving on the left all come back
 */
TEST(Starfield, StarsNeverLeaveTheWrapBand) {
  Starfield starfield(SEED);
  std::size_t outsideTheBand = 0;

  for (Duration left = ONE_MINUTE; left > Duration::zero();
       left -= MAXIMUM_BACKGROUND_STEP) {
    starfield.advance(MAXIMUM_BACKGROUND_STEP);
    for (const Star& star : starfield.stars()) {
      if (star.column < -STAR_WRAP_MARGIN ||
          star.column >=
              static_cast<int>(BACKGROUND_WIDTH) + STAR_WRAP_MARGIN) {
        ++outsideTheBand;
      }
    }
  }

  EXPECT_EQ(outsideTheBand, 0U);
}

/**
 * Given a near star
 * When it goes past the left wrap margin
 * Then it comes back just beyond the right edge, within one step of the
 * right wrap margin
 */
TEST(Starfield, StarLeavingOnTheLeftComesBackOnTheRight) {
  Starfield starfield(SEED);
  const std::size_t nearStar = starfield.stars().size() - 1;
  ASSERT_EQ(starfield.stars()[nearStar].depth, StarDepth::Near);
  const float oneStep =
      speedOf(StarDepth::Near) * seconds(MAXIMUM_BACKGROUND_STEP);
  float previous = starfield.positionOf(starfield.stars()[nearStar]);

  for (Duration left = ONE_MINUTE; left > Duration::zero();
       left -= MAXIMUM_BACKGROUND_STEP) {
    starfield.advance(MAXIMUM_BACKGROUND_STEP);
    const float current = starfield.positionOf(starfield.stars()[nearStar]);
    if (current > previous) {
      EXPECT_GE(current, BACKGROUND_WIDTH + STAR_WRAP_MARGIN - oneStep);
      EXPECT_LT(current, BACKGROUND_WIDTH + STAR_WRAP_MARGIN);
      return;
    }
    previous = current;
  }
  FAIL() << "the near star never came back within a minute";
}

/**
 * Given a starfield
 * When one second passes
 * Then the planet has drifted left by its speed for one second
 */
TEST(Starfield, PlanetDriftsSlowlyToTheLeft) {
  Starfield starfield(SEED);
  const float before = starfield.planetLeft();

  advanceBy(starfield, ONE_SECOND);

  EXPECT_NEAR(starfield.planetLeft(), before - PLANET_SPEED,
              POSITION_TOLERANCE);
}

/**
 * Given a starfield
 * When the planet has gone entirely past the left edge
 * Then it comes back the reentry margin beyond the right edge
 */
TEST(Starfield, PlanetLeavingOnTheLeftComesBackOnTheRight) {
  Starfield starfield(SEED);
  float previous = starfield.planetLeft();

  for (Duration left = LONGER_THAN_A_PLANET_CROSSING; left > Duration::zero();
       left -= MAXIMUM_BACKGROUND_STEP) {
    starfield.advance(MAXIMUM_BACKGROUND_STEP);
    if (starfield.planetLeft() > previous) {
      EXPECT_FLOAT_EQ(starfield.planetLeft(),
                      BACKGROUND_WIDTH + PLANET_REENTRY_MARGIN);
      return;
    }
    previous = starfield.planetLeft();
  }
  FAIL() << "the planet never came back";
}

/**
 * Given a new starfield
 * When its far plane is painted
 * Then it holds every far star and nothing else
 */
TEST(Starfield, TheFarPlaneHoldsTheFarStars) {
  const Starfield starfield(SEED);
  RecordingPixelSurface surface;

  starfield.paint(StarPlane::FarStars, surface);

  EXPECT_EQ(surface.plots().size(), countAt(starfield, StarDepth::Far));
  for (const ColoredPixel& plot : surface.plots()) {
    EXPECT_TRUE(plot.color == DEEP_BLUE || plot.color == BLUE);
  }
}

/**
 * Given a new starfield
 * When its planet plane is painted
 * Then it holds the pixels of the planet image, and nothing else
 */
TEST(Starfield, ThePlanetPlaneHoldsThePlanet) {
  const Starfield starfield(SEED);
  const PlanetImage planet;
  RecordingPixelSurface surface;

  starfield.paint(StarPlane::Planet, surface);

  ASSERT_EQ(surface.plots().size(), planet.pixels().size());
  for (std::size_t i = 0; i < planet.pixels().size(); ++i) {
    EXPECT_EQ(surface.plots()[i].color, planet.pixels()[i].color);
  }
}

/**
 * Given a starfield that moved for a tenth of a second
 * When its far plane is painted
 * Then each far star lies, to the hundredth of a pixel, the shift of the plane
 * right of the pixel it is painted on
 */
TEST(Starfield, AFarStarLiesTheShiftOfItsPlaneRightOfItsPixel) {
  Starfield starfield(SEED);
  advanceBy(starfield, A_TENTH_OF_A_SECOND);
  RecordingPixelSurface surface;

  starfield.paint(StarPlane::FarStars, surface);

  const float shift = starfield.shiftOf(StarPlane::FarStars);
  ASSERT_EQ(surface.plots().size(), countAt(starfield, StarDepth::Far));
  for (std::size_t i = 0; i < surface.plots().size(); ++i) {
    EXPECT_NEAR(static_cast<float>(surface.plots()[i].position.x) + shift,
                starfield.positionOf(starfield.stars()[i]),
                SUB_PIXEL_TOLERANCE);
  }
}

/**
 * Given a starfield that moved for a tenth of a second
 * When its planet plane is painted
 * Then the planet lies, to the hundredth of a pixel, the shift of its plane
 * right of the pixels it is painted on
 */
TEST(Starfield, ThePlanetLiesTheShiftOfItsPlaneRightOfItsPixels) {
  Starfield starfield(SEED);
  advanceBy(starfield, A_TENTH_OF_A_SECOND);
  const PlanetImage planet;
  RecordingPixelSurface surface;

  starfield.paint(StarPlane::Planet, surface);

  ASSERT_FALSE(surface.plots().empty());
  const int left =
      surface.plots().front().position.x - planet.pixels().front().position.x;
  EXPECT_NEAR(static_cast<float>(left) + starfield.shiftOf(StarPlane::Planet),
              starfield.planetLeft(), SUB_PIXEL_TOLERANCE);
}

/**
 * Given a starfield that moved for a tenth of a second
 * When the shift of its near plane is asked for
 * Then it is none: the near stars keep the grid of the playfield
 */
TEST(Starfield, TheNearPlaneKeepsTheGridOfThePlayfield) {
  Starfield starfield(SEED);
  advanceBy(starfield, A_TENTH_OF_A_SECOND);

  EXPECT_FLOAT_EQ(starfield.shiftOf(StarPlane::NearStars), 0.0F);
}

/**
 * Given a starfield
 * When its first far star is painted in the middle of each of eight
 * successive twinkle steps
 * Then it twinkles at exactly one of them
 */
TEST(Starfield, FarStarTwinklesOneStepInEight) {
  Starfield starfield(SEED);
  advanceBy(starfield, HALF_TWINKLE_STEP);
  std::int64_t twinkles = 0;

  for (std::int64_t step = 0; step < STAR_TWINKLE_CYCLE_STEPS; ++step) {
    RecordingPixelSurface surface;
    starfield.paint(StarPlane::FarStars, surface);
    if (surface.plots().front().color == BLUE) {
      ++twinkles;
    }
    advanceBy(starfield, ONE_TWINKLE_STEP);
  }

  EXPECT_EQ(twinkles, 1);
}

/**
 * Given two skies of the same seed, one going 12 times its speed
 * When 50 ms pass for both
 * Then each near star far from the left edge moved 12 times as far in the
 * fast sky
 */
TEST(Starfield, ASpeedFactorMovesTheStarsThatManyTimesFaster) {
  Starfield calm(SEED);
  Starfield fast(SEED);
  fast.setSpeedFactor(TOP_WARP_SPEED);
  const std::vector<float> before = positionsOf(calm);

  calm.advance(MAXIMUM_BACKGROUND_STEP);
  fast.advance(MAXIMUM_BACKGROUND_STEP);

  for (std::size_t i = 0; i < before.size(); ++i) {
    if (calm.stars()[i].depth != StarDepth::Near ||
        before[i] < FAR_FROM_THE_LEFT_EDGE) {
      continue;
    }
    SCOPED_TRACE(i);
    const float calmStep = before[i] - calm.positionOf(calm.stars()[i]);
    const float fastStep = before[i] - fast.positionOf(fast.stars()[i]);
    EXPECT_NEAR(fastStep, TOP_WARP_SPEED * calmStep, POSITION_TOLERANCE);
  }
}

/**
 * Given a sky going 12 times its speed
 * When it is painted
 * Then each near star has 6 pixels of streak at its right, the distance it
 * covers in a sixtieth of a second
 */
TEST(Starfield, AFastStarIsDrawnWithItsStreak) {
  Starfield starfield(SEED);
  starfield.setSpeedFactor(TOP_WARP_SPEED);
  RecordingPixelSurface surface;

  starfield.paint(StarPlane::NearStars, surface);

  const sf::Color streak =
      STAR_STREAK_COLORS[static_cast<std::size_t>(StarDepth::Near)];
  for (const Star& star : starfield.stars()) {
    if (star.depth != StarDepth::Near) {
      continue;
    }
    for (int column = 1; column <= NEAR_STREAK_AT_THE_TOP_WARP; ++column) {
      const sf::Vector2i pixel(star.column + column,
                               static_cast<int>(std::lround(star.height)));
      EXPECT_TRUE(std::ranges::any_of(
          surface.plots(), [pixel, streak](const ColoredPixel& plot) {
            return plot.position == pixel && plot.color == streak;
          }));
    }
  }
}

/**
 * Given the same sky painted at its own speed, at 1.5 and at 1.6 times its
 * speed
 * When the pixels painted are counted
 * Then 1.5 paints as many as its own speed, and 1.6 more: the streaks start
 * above 1.5
 */
TEST(Starfield, TheStreaksStartAboveOneAndAHalfTimesTheSpeed) {
  const std::size_t calm = plotCountAt(OWN_SPEED);

  EXPECT_EQ(plotCountAt(LAST_FACTOR_WITHOUT_STREAK), calm);
  EXPECT_GT(plotCountAt(FIRST_FACTOR_WITH_STREAKS), calm);
}
