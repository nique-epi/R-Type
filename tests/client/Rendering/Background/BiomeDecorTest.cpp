#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "BiomeDecor.hpp"
#include "BiomeDecorTestConstants.hpp"
#include "SkyLevel.hpp"

using rtype::client::arrivingShare;
using rtype::client::BiomeDecor;
using rtype::client::decorTileColumns;
using rtype::client::layersOfBiome;
using rtype::client::SkyLevel;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a picture two skies wide that has not scrolled
 * When its copies are placed
 * Then one copy starts at the left edge and covers the sky
 */
TEST(BiomeDecor, CoversTheSkyWithOneCopyBeforeAnyScroll) {
  EXPECT_THAT(decorTileColumns(NO_SCROLL, PICTURE_TWO_SKIES_WIDE),
              ElementsAre(LEFT_EDGE_OF_THE_SKY));
}

/**
 * Given a picture two skies wide scrolled 100 pixels
 * When its copies are placed
 * Then its one copy starts 100 pixels left of the sky
 */
TEST(BiomeDecor, MovesTheCopyLeftByTheScroll) {
  EXPECT_THAT(
      decorTileColumns(SCROLL_OF_A_HUNDRED_PIXELS, PICTURE_TWO_SKIES_WIDE),
      ElementsAre(A_HUNDRED_PIXELS_LEFT_OF_THE_SKY));
}

/**
 * Given a picture two skies wide scrolled 100.9 pixels
 * When its copies are placed
 * Then the copy starts on a whole pixel, 100 pixels left of the sky
 */
TEST(BiomeDecor, PlacesTheCopiesOnWholePixels) {
  EXPECT_THAT(decorTileColumns(SCROLL_OF_A_HUNDRED_PIXELS_AND_MORE,
                               PICTURE_TWO_SKIES_WIDE),
              ElementsAre(A_HUNDRED_PIXELS_LEFT_OF_THE_SKY));
}

/**
 * Given a picture two skies wide scrolled 600 pixels, whose first copy ends
 * 360 pixels into the sky
 * When its copies are placed
 * Then a second copy starts where the first ends
 */
TEST(BiomeDecor, AddsACopyWhereTheFirstEndsInsideTheSky) {
  EXPECT_THAT(
      decorTileColumns(SCROLL_OF_SIX_HUNDRED_PIXELS, PICTURE_TWO_SKIES_WIDE),
      ElementsAre(SIX_HUNDRED_PIXELS_LEFT_OF_THE_SKY,
                  SECOND_COPY_AFTER_SIX_HUNDRED_PIXELS));
}

/**
 * Given a picture scrolled by its whole width
 * When its copies are placed
 * Then they are back where they started
 */
TEST(BiomeDecor, StartsAgainOnceThePictureScrolledWhole) {
  EXPECT_THAT(
      decorTileColumns(SCROLL_OF_THE_WHOLE_PICTURE, PICTURE_TWO_SKIES_WIDE),
      ElementsAre(LEFT_EDGE_OF_THE_SKY));
}

/**
 * Given a picture 200 pixels wide on a sky of 480
 * When its copies are placed
 * Then three copies cover the sky
 */
TEST(BiomeDecor, RepeatsAPictureNarrowerThanTheSky) {
  EXPECT_THAT(
      decorTileColumns(NO_SCROLL, PICTURE_NARROWER_THAN_THE_SKY),
      ElementsAre(LEFT_EDGE_OF_THE_SKY, SECOND_NARROW_COPY, THIRD_NARROW_COPY));
}

/**
 * Given a picture without width
 * When its copies are placed
 * Then there is none
 */
TEST(BiomeDecor, PlacesNoCopyOfAPictureWithoutWidth) {
  EXPECT_THAT(decorTileColumns(NO_SCROLL, PICTURE_WITHOUT_WIDTH), IsEmpty());
}

/**
 * Given a biome the client has a decor for
 * When its layers are asked for
 * Then it has its four layers
 */
TEST(BiomeDecor, FindsTheLayersOfABiome) {
  EXPECT_EQ(layersOfBiome(KNOWN_BIOME).size(), LAYERS_OF_THE_KNOWN_BIOME);
}

/**
 * Given a name no biome has
 * When its layers are asked for
 * Then there is none
 */
TEST(BiomeDecor, HasNoLayerForAnUnknownBiome) {
  EXPECT_TRUE(layersOfBiome(UNKNOWN_BIOME).empty());
}

/**
 * Given a decor shown a level scrolled 100 units that scrolls 120 units per
 * second
 * When one second passes
 * Then the decor shows the level 220 units in
 */
TEST(BiomeDecor, MovesOnAtTheSpeedOfTheLevel) {
  BiomeDecor decor{[](std::string_view) { return nullptr; }};
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
  BiomeDecor decor{[](std::string_view) { return nullptr; }};
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
  BiomeDecor decor{[](std::string_view) { return nullptr; }};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SHOWN_SCROLL_POSITION,
                      .scrollSpeed = SHOWN_SCROLL_SPEED});

  decor.advance(ONE_SECOND_BACKWARDS_OF_DECOR);

  EXPECT_FLOAT_EQ(decor.scrollPosition(), SHOWN_SCROLL_POSITION);
}

/**
 * Given the third biome of the levels
 * When its layers are asked for
 * Then it has its four layers
 */
TEST(BiomeDecor, FindsTheLayersOfTheThirdBiome) {
  EXPECT_EQ(layersOfBiome(THIRD_BIOME).size(), LAYERS_OF_THE_KNOWN_BIOME);
}

/**
 * Given a fade of one second between two biomes
 * When the share of the arriving biome is read along it
 * Then it is 0 at its start, half halfway, and whole at its end
 */
TEST(BiomeDecor, ShowsTheArrivingBiomeAlongTheFade) {
  EXPECT_FLOAT_EQ(arrivingShare(WHOLE_FADE_LEFT), NOT_SHOWN_YET);
  EXPECT_FLOAT_EQ(arrivingShare(HALF_THE_FADE), HALF_SHOWN);
  EXPECT_FLOAT_EQ(arrivingShare(NO_FADE_LEFT), FULLY_SHOWN);
}

/**
 * Given a time left of the fade below 0 or beyond the whole fade
 * When the share of the arriving biome is read
 * Then it is that of the nearest end of the fade
 */
TEST(BiomeDecor, CountsATimeOutsideTheFadeAsItsNearestEnd) {
  EXPECT_FLOAT_EQ(arrivingShare(FADE_OVER_A_SECOND_AGO), FULLY_SHOWN);
  EXPECT_FLOAT_EQ(arrivingShare(TWICE_THE_FADE_LEFT), NOT_SHOWN_YET);
}

/**
 * Given a decor that shows a biome whole
 * When another biome is shown
 * Then the new biome starts hidden, is half shown halfway through the fade,
 * and whole at its end
 */
TEST(BiomeDecor, FadesANewBiomeIn) {
  BiomeDecor decor{[](std::string_view) { return nullptr; }};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SHOWN_SCROLL_POSITION,
                      .scrollSpeed = SHOWN_SCROLL_SPEED});
  decor.advance(ONE_SECOND_OF_DECOR);

  decor.show(SkyLevel{.biome = std::string{OTHER_KNOWN_BIOME},
                      .scrollPosition = SHOWN_SCROLL_POSITION,
                      .scrollSpeed = SHOWN_SCROLL_SPEED});
  EXPECT_FLOAT_EQ(decor.shownShare(), NOT_SHOWN_YET);

  decor.advance(HALF_THE_FADE);
  EXPECT_FLOAT_EQ(decor.shownShare(), HALF_SHOWN);

  decor.advance(ONE_SECOND_OF_DECOR);
  EXPECT_FLOAT_EQ(decor.shownShare(), FULLY_SHOWN);
}

/**
 * Given a decor that shows a biome whole
 * When the same biome is shown again, further on
 * Then it stays whole and takes the new scroll
 */
TEST(BiomeDecor, KeepsTheBiomeShownWhenTheSameComesAgain) {
  BiomeDecor decor{[](std::string_view) { return nullptr; }};
  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = SHOWN_SCROLL_POSITION,
                      .scrollSpeed = SHOWN_SCROLL_SPEED});
  decor.advance(ONE_SECOND_OF_DECOR);

  decor.show(SkyLevel{.biome = std::string{KNOWN_BIOME},
                      .scrollPosition = LATER_SCROLL_POSITION,
                      .scrollSpeed = SHOWN_SCROLL_SPEED});

  EXPECT_FLOAT_EQ(decor.shownShare(), FULLY_SHOWN);
  EXPECT_FLOAT_EQ(decor.scrollPosition(), LATER_SCROLL_POSITION);
}
