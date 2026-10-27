#include <gtest/gtest.h>
#include <random>
#include "BackgroundTestConstants.hpp"
#include "Flare.hpp"
#include "FlareTestConstants.hpp"
#include "RecordingPixelSurface.hpp"
#include "SkyEventConstants.hpp"
#include "SkyEventFixtures.hpp"
#include "SkyEventTestConstants.hpp"

using rtype::client::Flare;
using rtype::client::FLARE_LONGEST_DURATION;
using rtype::client::FLARE_SHORTEST_DURATION;

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
