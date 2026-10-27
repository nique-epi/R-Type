#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <random>
#include "BackgroundTestConstants.hpp"
#include "RecordingPixelSurface.hpp"
#include "ShootingStar.hpp"
#include "ShootingStarTestConstants.hpp"
#include "SkyEventConstants.hpp"
#include "SkyEventFixtures.hpp"
#include "SkyEventTestConstants.hpp"
#include "SkyLight.hpp"

using rtype::client::SHOOTING_STAR_LONGEST_LIFE;
using rtype::client::SHOOTING_STAR_LONGEST_TRAIL;
using rtype::client::SHOOTING_STAR_SHORTEST_LIFE;
using rtype::client::SHOOTING_STAR_TRAIL_SHRINK;
using rtype::client::ShootingStar;
using rtype::client::SkyLight;

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
  const sf::Vector2i before =
      paintOf(star, SkyLight::Twinkling).plots().front().position;

  advanceFor(star, A_TENTH_OF_A_SECOND);

  const sf::Vector2i after =
      paintOf(star, SkyLight::Twinkling).plots().front().position;
  EXPECT_LT(after.x, before.x);
  EXPECT_GT(after.y, before.y);
}
