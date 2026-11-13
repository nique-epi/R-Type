#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include "BiomeDecor.hpp"
#include "BiomeDecorFixtures.hpp"
#include "BiomeDecorTestConstants.hpp"
#include "SkyLevel.hpp"

using rtype::client::BiomeDecor;
using rtype::client::biomeIndexOf;
using rtype::client::SkyLevel;
using ::testing::Optional;

/**
 * Given the biome the client has a decor for
 * When its rank is asked for
 * Then it is its rank among the biomes
 */
TEST(BiomeDecor, FindsTheRankOfABiome) {
  EXPECT_THAT(biomeIndexOf(KNOWN_BIOME), Optional(RANK_OF_THE_KNOWN_BIOME));
}

/**
 * Given a name no biome has
 * When its rank is asked for
 * Then there is none
 */
TEST(BiomeDecor, HasNoRankForAnUnknownBiome) {
  EXPECT_FALSE(biomeIndexOf(UNKNOWN_BIOME).has_value());
}

/**
 * Given the assets folder of the repository
 * When a decor is built from it
 * Then it read the maps of the Pit
 */
TEST(BiomeDecor, ReadsTheMapsOfTheBiomesOfTheAssets) {
  const BiomeDecor decor{noTexture(), filesOfTheAssets()};

  EXPECT_TRUE(decor.hasMapsOf(KNOWN_BIOME));
}

/**
 * Given an assets folder without the project of the Pit
 * When a decor is built from it
 * Then it is built, without the maps of the Pit
 */
TEST(BiomeDecor, ShowsNoDecorForABiomeWhoseProjectIsMissing) {
  const BiomeDecor decor{noTexture(), filesNowhere()};

  EXPECT_FALSE(decor.hasMapsOf(KNOWN_BIOME));
}

/**
 * Given a decor shown a level scrolled 100 units that scrolls 120 units per
 * second
 * When one second passes
 * Then the decor shows the level 220 units in
 */
TEST(BiomeDecor, MovesOnAtTheSpeedOfTheLevel) {
  BiomeDecor decor{noTexture(), filesNowhere()};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SHOWN_SCROLL_POSITION,
                      .scrollSpeed = SHOWN_SCROLL_SPEED});

  decor.advance(ONE_SECOND_OF_DECOR);

  EXPECT_FLOAT_EQ(decor.scrollPosition(), SCROLL_POSITION_ONE_SECOND_LATER);
}

/**
 * Given a decor shown a level whose scroll is paused
 * When one second passes
 * Then the decor has not moved
 */
TEST(BiomeDecor, StaysStillWhileTheScrollIsPaused) {
  BiomeDecor decor{noTexture(), filesNowhere()};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SHOWN_SCROLL_POSITION,
                      .scrollSpeed = PAUSED_SCROLL});

  decor.advance(ONE_SECOND_OF_DECOR);

  EXPECT_FLOAT_EQ(decor.scrollPosition(), SHOWN_SCROLL_POSITION);
}

/**
 * Given a decor shown a level that scrolls
 * When a negative time passes
 * Then the decor has not moved
 */
TEST(BiomeDecor, DoesNotMoveBackwards) {
  BiomeDecor decor{noTexture(), filesNowhere()};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SHOWN_SCROLL_POSITION,
                      .scrollSpeed = SHOWN_SCROLL_SPEED});

  decor.advance(ONE_SECOND_BACKWARDS_OF_DECOR);

  EXPECT_FLOAT_EQ(decor.scrollPosition(), SHOWN_SCROLL_POSITION);
}
