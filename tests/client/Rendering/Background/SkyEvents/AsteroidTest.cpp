#include <gtest/gtest.h>
#include <random>
#include "Asteroid.hpp"
#include "AsteroidTestConstants.hpp"
#include "BackgroundTestConstants.hpp"
#include "SkyEventConstants.hpp"
#include "SkyEventFixtures.hpp"
#include "SkyEventTestConstants.hpp"

using rtype::client::Asteroid;
using rtype::client::ASTEROID_FASTEST_SPEED;
using rtype::client::ASTEROID_SLOWEST_SPEED;

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
