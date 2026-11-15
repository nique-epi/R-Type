#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include "BackgroundTestConstants.hpp"
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

/**
 * Given a decor shown a level of the Pit scrolled 102 units since the Pit
 * began
 * When the shifts of its far and middle planes are asked for
 * Then each lies left of its whole pixels by what it entered past them:
 * 0.375 and 0.75 of a pixel
 */
TEST(BiomeDecor, ShiftsASlowPlaneByWhatItEnteredPastItsPixels) {
  BiomeDecor decor{noTexture(), filesNowhere()};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SCROLL_OFF_THE_GRID,
                      .scrollSpeed = PAUSED_SCROLL,
                      .biomeSince = BIOME_BEGAN_AT_THE_START});

  EXPECT_FLOAT_EQ(decor.layerShift(FAR_PLANE), FAR_PLANE_SHIFT_OFF_THE_GRID);
  EXPECT_FLOAT_EQ(decor.layerShift(MIDDLE_PLANE),
                  MIDDLE_PLANE_SHIFT_OFF_THE_GRID);
}

/**
 * Given a decor shown a level of the Pit whose near plane entered 25.5 pixels
 * When the shift of that plane is asked for
 * Then it is none: the near plane keeps the grid of the playfield
 */
TEST(BiomeDecor, KeepsTheNearPlaneOnTheGridOfThePlayfield) {
  BiomeDecor decor{noTexture(), filesNowhere()};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SCROLL_OFF_THE_GRID,
                      .scrollSpeed = PAUSED_SCROLL,
                      .biomeSince = BIOME_BEGAN_AT_THE_START});

  EXPECT_FLOAT_EQ(decor.layerShift(NEAR_PLANE), 0.0F);
}

/**
 * Given a decor shown a level of a biome it has no decor for
 * When the shift of a plane is asked for
 * Then it is none
 */
TEST(BiomeDecor, ShiftsNoPlaneOfAnUnknownBiome) {
  BiomeDecor decor{noTexture(), filesNowhere()};
  decor.show(SkyLevel{.biome = std::string{UNKNOWN_BIOME},
                      .scrollPosition = SCROLL_OFF_THE_GRID,
                      .scrollSpeed = PAUSED_SCROLL});

  EXPECT_FLOAT_EQ(decor.layerShift(FAR_PLANE), 0.0F);
}

/**
 * Given a decor of the assets shown a level of the Pit that just began
 * When its seam is asked for
 * Then it lies where the first near tile of the Pit enters: column 592
 */
TEST(BiomeDecor, PlacesTheSeamWhereTheEntranceBegins) {
  BiomeDecor decor{noTexture(), filesOfTheAssets()};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SCROLL_AT_THE_START_OF_THE_PIT,
                      .scrollSpeed = PAUSED_SCROLL,
                      .biomeSince = BIOME_BEGAN_AT_THE_START});

  EXPECT_THAT(decor.seamColumn(), Optional(SEAM_AT_THE_START_OF_THE_PIT));
}

/**
 * Given a decor of the assets shown a level of the Pit whose near plane
 * entered 100 pixels
 * When its seam is asked for
 * Then it moved 100 columns left with the near plane
 */
TEST(BiomeDecor, MovesTheSeamWithTheNearPlane) {
  BiomeDecor decor{noTexture(), filesOfTheAssets()};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SCROLL_OF_A_HUNDRED_NEAR_PIXELS,
                      .scrollSpeed = PAUSED_SCROLL,
                      .biomeSince = BIOME_BEGAN_AT_THE_START});

  EXPECT_THAT(decor.seamColumn(), Optional(SEAM_A_HUNDRED_PIXELS_IN));
}

/**
 * Given a decor of the assets shown a level of the Pit whose near plane
 * entered 100.5 pixels
 * When its seam is asked for
 * Then it lies on the whole column of 100 pixels in, as the near plane does
 */
TEST(BiomeDecor, PlacesTheSeamOnAWholeColumn) {
  BiomeDecor decor{noTexture(), filesOfTheAssets()};
  decor.show(
      SkyLevel{.biome = std::string{KNOWN_BIOME},
               .scrollPosition = SCROLL_OF_A_HUNDRED_AND_A_HALF_NEAR_PIXELS,
               .scrollSpeed = PAUSED_SCROLL,
               .biomeSince = BIOME_BEGAN_AT_THE_START});

  EXPECT_THAT(decor.seamColumn(), Optional(SEAM_A_HUNDRED_PIXELS_IN));
}

/**
 * Given a decor without the project of the Pit, shown a level of the Pit
 * When its seam is asked for
 * Then there is none
 */
TEST(BiomeDecor, HasNoSeamWithoutMaps) {
  BiomeDecor decor{noTexture(), filesNowhere()};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME}});

  EXPECT_FALSE(decor.seamColumn().has_value());
}
