#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "ColoredPixel.hpp"
#include "PlanetImage.hpp"
#include "RecordingPixelSurface.hpp"
#include "Star.hpp"
#include "StarTwinkleStep.hpp"
#include "Starfield.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::BACKGROUND_HEIGHT;
using rtype::client::BACKGROUND_WIDTH;
using rtype::client::BLUE;
using rtype::client::ColoredPixel;
using rtype::client::DEEP_BLUE;
using rtype::client::MAXIMUM_BACKGROUND_STEP;
using rtype::client::PALE_BLUE;
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
using rtype::client::StarTwinkleStep;
using rtype::engine::Duration;
using rtype::engine::testing::ONE_SECOND;

namespace {

constexpr std::uint32_t SEED = 2026;
constexpr Duration SHORT_STEP = std::chrono::milliseconds(10);
constexpr Duration HALF_MAXIMUM_STEP = MAXIMUM_BACKGROUND_STEP / 2;
constexpr Duration ONE_MINUTE = std::chrono::minutes(1);
constexpr Duration NEGATIVE_STEP = -std::chrono::milliseconds(10);
constexpr Duration ONE_TWINKLE_STEP =
    std::chrono::duration_cast<Duration>(StarTwinkleStep(1));
constexpr Duration HALF_TWINKLE_STEP = ONE_TWINKLE_STEP / 2;
constexpr Duration LONGER_THAN_A_PLANET_CROSSING = std::chrono::seconds(260);
constexpr float POSITION_TOLERANCE = 1e-3F;
constexpr float TOP_WARP_FACTOR = 12.0F;
constexpr float LAST_FACTOR_WITHOUT_STREAK = 1.5F;
constexpr float FIRST_FACTOR_WITH_STREAKS = 1.6F;
constexpr int NEAR_STREAK_AT_THE_TOP_WARP = 6;
constexpr float FAR_FROM_THE_LEFT_EDGE = 40.0F;

float seconds(Duration duration) {
  return std::chrono::duration<float>(duration).count();
}

void advanceBy(Starfield& starfield, Duration total) {
  for (Duration left = total; left > Duration::zero();
       left -= MAXIMUM_BACKGROUND_STEP) {
    starfield.advance(std::min(left, MAXIMUM_BACKGROUND_STEP));
  }
}

float speedOf(StarDepth depth) {
  return STAR_LAYERS[static_cast<std::size_t>(depth)].speed;
}

std::size_t countAt(const Starfield& starfield, StarDepth depth) {
  return static_cast<std::size_t>(
      std::ranges::count(starfield.stars(), depth, &Star::depth));
}

void expectSameColumns(const Starfield& left, const Starfield& right) {
  ASSERT_EQ(left.stars().size(), right.stars().size());
  for (std::size_t i = 0; i < left.stars().size(); ++i) {
    EXPECT_NEAR(left.stars()[i].position.x, right.stars()[i].position.x,
                POSITION_TOLERANCE);
  }
}

/** @returns How many pixels a starfield of SEED paints at that speed
 * factor. */
std::size_t plotCountAt(float speedFactor) {
  Starfield starfield(SEED);
  starfield.setSpeedFactor(speedFactor);
  RecordingPixelSurface surface;
  starfield.paint(surface);
  return surface.plots().size();
}

}  // namespace

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
    EXPECT_GE(star.position.x, 0.0F);
    EXPECT_LE(star.position.x, BACKGROUND_WIDTH);
    EXPECT_GE(star.position.y, 0.0F);
    EXPECT_LE(star.position.y, BACKGROUND_HEIGHT);
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

  starfield.advance(SHORT_STEP);

  for (std::size_t i = 0; i < before.size(); ++i) {
    const Star& star = starfield.stars()[i];
    EXPECT_NEAR(
        star.position.x,
        before[i].position.x - (speedOf(star.depth) * seconds(SHORT_STEP)),
        POSITION_TOLERANCE);
    EXPECT_EQ(star.position.y, before[i].position.y);
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
 * Then no star ever leaves the band between the two wrap margins: stars
 * leaving on the left all come back
 */
TEST(Starfield, StarsNeverLeaveTheWrapBand) {
  Starfield starfield(SEED);
  std::size_t outsideTheBand = 0;

  for (Duration left = ONE_MINUTE; left > Duration::zero();
       left -= MAXIMUM_BACKGROUND_STEP) {
    starfield.advance(MAXIMUM_BACKGROUND_STEP);
    for (const Star& star : starfield.stars()) {
      if (star.position.x < -STAR_WRAP_MARGIN ||
          star.position.x >= BACKGROUND_WIDTH + STAR_WRAP_MARGIN) {
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
  float previous = starfield.stars()[nearStar].position.x;

  for (Duration left = ONE_MINUTE; left > Duration::zero();
       left -= MAXIMUM_BACKGROUND_STEP) {
    starfield.advance(MAXIMUM_BACKGROUND_STEP);
    const float current = starfield.stars()[nearStar].position.x;
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
 * When it is painted
 * Then the far stars come first, the planet right after them, and the middle
 * stars after the planet
 */
TEST(Starfield, PlanetIsPaintedBetweenFarAndNearerStars) {
  const Starfield starfield(SEED);
  const PlanetImage planet;
  RecordingPixelSurface surface;

  starfield.paint(surface);

  const std::size_t farStars = countAt(starfield, StarDepth::Far);
  const std::size_t planetEnd = farStars + planet.pixels().size();
  ASSERT_GT(surface.plots().size(), planetEnd);
  for (std::size_t i = 0; i < farStars; ++i) {
    const sf::Color color = surface.plots()[i].color;
    EXPECT_TRUE(color == DEEP_BLUE || color == BLUE);
  }
  for (std::size_t i = farStars; i < planetEnd; ++i) {
    EXPECT_EQ(surface.plots()[i].color, planet.pixels()[i - farStars].color);
  }
  const sf::Color firstMiddleStar = surface.plots()[planetEnd].color;
  EXPECT_TRUE(firstMiddleStar == PALE_BLUE || firstMiddleStar == BLUE);
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
    starfield.paint(surface);
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
  fast.setSpeedFactor(TOP_WARP_FACTOR);
  const std::vector<Star> before = calm.stars();

  calm.advance(MAXIMUM_BACKGROUND_STEP);
  fast.advance(MAXIMUM_BACKGROUND_STEP);

  for (std::size_t i = 0; i < before.size(); ++i) {
    if (before[i].depth != StarDepth::Near ||
        before[i].position.x < FAR_FROM_THE_LEFT_EDGE) {
      continue;
    }
    SCOPED_TRACE(i);
    const float calmStep = before[i].position.x - calm.stars()[i].position.x;
    const float fastStep = before[i].position.x - fast.stars()[i].position.x;
    EXPECT_NEAR(fastStep, TOP_WARP_FACTOR * calmStep, POSITION_TOLERANCE);
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
  starfield.setSpeedFactor(TOP_WARP_FACTOR);
  RecordingPixelSurface surface;

  starfield.paint(surface);

  const sf::Color streak =
      STAR_STREAK_COLORS[static_cast<std::size_t>(StarDepth::Near)];
  for (const Star& star : starfield.stars()) {
    if (star.depth != StarDepth::Near) {
      continue;
    }
    for (int column = 1; column <= NEAR_STREAK_AT_THE_TOP_WARP; ++column) {
      const sf::Vector2i pixel(
          static_cast<int>(std::lround(star.position.x)) + column,
          static_cast<int>(std::lround(star.position.y)));
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
  const std::size_t calm = plotCountAt(1.0F);

  EXPECT_EQ(plotCountAt(LAST_FACTOR_WITHOUT_STREAK), calm);
  EXPECT_GT(plotCountAt(FIRST_FACTOR_WITH_STREAKS), calm);
}
