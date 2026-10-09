#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include "Asteroid.hpp"
#include "BackgroundConstants.hpp"
#include "Comet.hpp"
#include "Flare.hpp"
#include "ISkyEvent.hpp"
#include "RecordingPixelSurface.hpp"
#include "ShootingStar.hpp"
#include "SkyEventConstants.hpp"

using rtype::client::Asteroid;
using rtype::client::ASTEROID_FASTEST_SPEED;
using rtype::client::ASTEROID_OFF_SKY_MARGIN;
using rtype::client::ASTEROID_SLOWEST_SPEED;
using rtype::client::BACKGROUND_WIDTH;
using rtype::client::Comet;
using rtype::client::COMET_ENTRY_MARGIN;
using rtype::client::COMET_FASTEST_SPEED;
using rtype::client::COMET_HEAD_OFFSETS;
using rtype::client::COMET_OFF_SKY_MARGIN;
using rtype::client::COMET_SLOWEST_SPEED;
using rtype::client::CROSS_DIRECTIONS;
using rtype::client::DIAGONAL_DIRECTIONS;
using rtype::client::Flare;
using rtype::client::FLARE_LONGEST_DURATION;
using rtype::client::FLARE_SHORTEST_DURATION;
using rtype::client::ISkyEvent;
using rtype::client::SHOOTING_STAR_LONGEST_LIFE;
using rtype::client::SHOOTING_STAR_LONGEST_TRAIL;
using rtype::client::SHOOTING_STAR_SHORTEST_LIFE;
using rtype::client::SHOOTING_STAR_TRAIL_SHRINK;
using rtype::client::ShootingStar;

namespace {

constexpr std::uint32_t SEED = 2026;
/** Seeds tried when a property must hold whatever life a star draws. */
constexpr std::uint32_t SEED_COUNT = 20;
constexpr float FRAME = 0.01F;
constexpr float A_TENTH_OF_A_SECOND = 0.1F;
constexpr float MIDDLE_OF_ANY_FLARE = 1.0F;
constexpr float ONE_SECOND = 1.0F;
/** Longest flare arm, FLARE_LONGEST_ARM rounded to whole pixels. */
constexpr std::size_t FULL_ARM_LENGTH = 3;
constexpr std::size_t PIXELS_OF_A_FULL_FLARE =
    (CROSS_DIRECTIONS.size() * FULL_ARM_LENGTH) + DIAGONAL_DIRECTIONS.size() +
    1;
constexpr float ASTEROID_CROSSING =
    BACKGROUND_WIDTH + (2.0F * ASTEROID_OFF_SKY_MARGIN);
constexpr float COMET_CROSSING =
    BACKGROUND_WIDTH + COMET_ENTRY_MARGIN + COMET_OFF_SKY_MARGIN;

std::mt19937 generatorFrom(std::uint32_t seed) { return std::mt19937(seed); }

void advanceFor(ISkyEvent& event, float eventSeconds) {
  const auto frames = static_cast<int>(std::ceil(eventSeconds / FRAME));
  for (int frame = 0; frame < frames; ++frame) {
    event.advance(FRAME);
  }
}

RecordingPixelSurface paintOf(const ISkyEvent& event) {
  RecordingPixelSurface surface;
  event.paint(surface);
  return surface;
}

}  // namespace

/**
 * Given new shooting stars drawn from twenty seeds, so that some draw a life
 * close to the shortest one
 * When less time than the shortest life passes
 * Then none of them is over
 */
TEST(ShootingStar, LastsAtLeastItsShortestLife) {
  for (std::uint32_t seed = SEED; seed < SEED + SEED_COUNT; ++seed) {
    std::mt19937 random = generatorFrom(seed);
    ShootingStar star(random);

    advanceFor(star, SHOOTING_STAR_SHORTEST_LIFE - FRAME);

    EXPECT_FALSE(star.isOver()) << "seed " << seed;
  }
}

/**
 * Given a new shooting star
 * When its longest life has passed and its longest trail has had time to
 * shrink
 * Then it is over
 */
TEST(ShootingStar, EndsOnceItsTrailHasShrunk) {
  std::mt19937 random = generatorFrom(SEED);
  ShootingStar star(random);

  advanceFor(star,
             SHOOTING_STAR_LONGEST_LIFE +
                 (SHOOTING_STAR_LONGEST_TRAIL / SHOOTING_STAR_TRAIL_SHRINK) +
                 FRAME);

  EXPECT_TRUE(star.isOver());
}

/**
 * Given a shooting star
 * When a tenth of a second passes
 * Then its head has moved left and down
 */
TEST(ShootingStar, FallsTowardsTheBottomLeft) {
  std::mt19937 random = generatorFrom(SEED);
  ShootingStar star(random);
  star.advance(FRAME);
  const sf::Vector2i before = paintOf(star).plots().front().position;

  advanceFor(star, A_TENTH_OF_A_SECOND);

  const sf::Vector2i after = paintOf(star).plots().front().position;
  EXPECT_LT(after.x, before.x);
  EXPECT_GT(after.y, before.y);
}

/**
 * Given a flare that has just started
 * When it is painted
 * Then it is a single pixel: its arms have not grown yet
 */
TEST(Flare, StartsAsASinglePixel) {
  std::mt19937 random = generatorFrom(SEED);
  Flare flare(random);

  flare.advance(FRAME);

  EXPECT_EQ(paintOf(flare).plots().size(), 1U);
}

/**
 * Given a flare in the middle of its life
 * When it is painted
 * Then it shows its full cross: arms of three pixels, diagonals and core
 */
TEST(Flare, SpreadsIntoAFullCrossMidLife) {
  std::mt19937 random = generatorFrom(SEED);
  Flare flare(random);

  advanceFor(flare, MIDDLE_OF_ANY_FLARE);

  EXPECT_EQ(paintOf(flare).plots().size(), PIXELS_OF_A_FULL_FLARE);
}

/**
 * Given a new flare
 * When less time than its shortest duration passes
 * Then it is not over
 */
TEST(Flare, LastsAtLeastItsShortestDuration) {
  std::mt19937 random = generatorFrom(SEED);
  Flare flare(random);

  advanceFor(flare, FLARE_SHORTEST_DURATION - FRAME);

  EXPECT_FALSE(flare.isOver());
}

/**
 * Given a new flare
 * When its longest duration has passed
 * Then it is over
 */
TEST(Flare, EndsWithinItsLongestDuration) {
  std::mt19937 random = generatorFrom(SEED);
  Flare flare(random);

  advanceFor(flare, FLARE_LONGEST_DURATION + FRAME);

  EXPECT_TRUE(flare.isOver());
}

/**
 * Given a new asteroid
 * When less time passes than its fastest crossing of the sky takes
 * Then it is not over
 */
TEST(Asteroid, StaysUntilItHasCrossedTheSky) {
  std::mt19937 random = generatorFrom(SEED);
  Asteroid asteroid(random);

  advanceFor(asteroid, (ASTEROID_CROSSING / ASTEROID_FASTEST_SPEED) - FRAME);

  EXPECT_FALSE(asteroid.isOver());
}

/**
 * Given a new asteroid
 * When its slowest crossing of the sky has had time to end
 * Then it is over
 */
TEST(Asteroid, EndsOnceItHasCrossedTheSky) {
  std::mt19937 random = generatorFrom(SEED);
  Asteroid asteroid(random);

  advanceFor(asteroid, (ASTEROID_CROSSING / ASTEROID_SLOWEST_SPEED) + FRAME);

  EXPECT_TRUE(asteroid.isOver());
}

/**
 * Given a comet in the sky
 * When it is painted
 * Then every pixel of its tail lies right of its head: the tail trails behind
 */
TEST(Comet, TailTrailsBehindTheHead) {
  std::mt19937 random = generatorFrom(SEED);
  Comet comet(random);
  advanceFor(comet, ONE_SECOND);

  const RecordingPixelSurface surface = paintOf(comet);

  ASSERT_GT(surface.plots().size(), COMET_HEAD_OFFSETS.size());
  const std::size_t tailCount =
      surface.plots().size() - COMET_HEAD_OFFSETS.size();
  const int headColumn = surface.plots()[tailCount].position.x;
  for (std::size_t i = 0; i < tailCount; ++i) {
    EXPECT_GE(surface.plots()[i].position.x, headColumn);
  }
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
