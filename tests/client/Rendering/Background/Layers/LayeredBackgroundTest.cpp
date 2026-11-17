#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include "BackgroundPace.hpp"
#include "ImageLayerFixtures.hpp"
#include "LayerFrame.hpp"
#include "LayerTestConstants.hpp"
#include "LayeredBackground.hpp"
#include "LayeredBackgroundFixture.hpp"
#include "LayeredBackgroundTestConstants.hpp"
#include "RecordingLayerCanvas.hpp"

using rtype::client::BackgroundPace;
using rtype::client::LayeredBackground;
using ::testing::ElementsAre;
using ::testing::Optional;

/**
 * Given a sky of two layers, and the Pit, of three
 * When each is built
 * Then each holds one layer per layer of its description
 */
TEST_F(LayeredBackgroundFixture, HoldsOneLayerPerLayerOfItsDescription) {
  const LayeredBackground sky = smallSky();
  const LayeredBackground pit = shipped(THE_PIT);

  EXPECT_EQ(sky.layerCount(), LAYERS_OF_THE_SMALL_SKY);
  EXPECT_EQ(pit.layerCount(), LAYERS_OF_THE_SHIPPED_PIT);
}

/**
 * Given the planet of the sky, whose layer has not moved
 * When it is painted
 * Then its image shows where its description puts it
 */
TEST_F(LayeredBackgroundFixture, PaintsALayerThroughWhatItShows) {
  const LayeredBackground sky = smallSky();
  RecordingLayerCanvas canvas;

  sky.paint(PLANET_LAYER_OF_THE_SKY, canvas);

  EXPECT_THAT(cornersOnTheBackground(canvas.frames()),
              ElementsAre(WHERE_THE_PLANET_STARTS));
}

/**
 * Given the planet, which drifts 1.8 pixels per second
 * When ten seconds pass
 * Then its layer lies 18 pixels to the left
 */
TEST_F(LayeredBackgroundFixture, ALayerMovesByItsAutoscrollOverTime) {
  LayeredBackground sky = smallSky();

  sky.advance(TEN_SECONDS_OF_LAYER, BackgroundPace{});

  EXPECT_NEAR(whereLies(sky, PLANET_LAYER_OF_THE_SKY), PLANET_AFTER_TEN_SECONDS,
              ROUNDING_OF_A_PLACE);
}

/**
 * Given the Pit, whose far walls follow a quarter of the scroll and its
 * middle walls half
 * When the level has scrolled 100 pixels since it began
 * Then they stand 25 and 50 columns to the left
 */
TEST_F(LayeredBackgroundFixture, ALayerFollowsItsShareOfTheScroll) {
  LayeredBackground pit = shipped(THE_PIT);

  pit.advance(ONE_FRAME_OF_LAYER, A_HUNDRED_PIXELS_IN);

  EXPECT_EQ(pit.frameOf(FAR_WALLS).column, FAR_WALLS_A_HUNDRED_PIXELS_IN);
  EXPECT_EQ(pit.frameOf(MIDDLE_WALLS).column, MIDDLE_WALLS_A_HUNDRED_PIXELS_IN);
}

/**
 * Given the Pit, which arrives through its near walls
 * When it begins, then once the level has scrolled 100 pixels
 * Then the near walls start on the right edge of the background, then stand
 * 100 columns left of it
 */
TEST_F(LayeredBackgroundFixture, TheEntranceStartsOnTheRightEdge) {
  LayeredBackground pit = shipped(THE_PIT);
  const int whenItBegins = pit.frameOf(NEAR_WALLS).column;

  pit.advance(ONE_FRAME_OF_LAYER, A_HUNDRED_PIXELS_IN);

  EXPECT_EQ(whenItBegins, RIGHT_EDGE_OF_THE_BACKGROUND);
  EXPECT_EQ(pit.frameOf(NEAR_WALLS).column, ENTRANCE_A_HUNDRED_PIXELS_IN);
}

/**
 * Given the far stars, which warp, and the planet, which does not
 * When ten seconds pass at four times the speed
 * Then the stars moved four times their 45 pixels, and the planet its 18
 */
TEST_F(LayeredBackgroundFixture, AWarpSpeedsUpOnlyTheLayersThatWarp) {
  LayeredBackground sky = smallSky();

  sky.advance(TEN_SECONDS_OF_LAYER, AT_FOUR_TIMES_THE_SPEED);

  EXPECT_NEAR(whereLies(sky, STAR_LAYER), STARS_AFTER_TEN_SECONDS_OF_WARP,
              ROUNDING_OF_A_PLACE);
  EXPECT_NEAR(whereLies(sky, PLANET_LAYER_OF_THE_SKY), PLANET_AFTER_TEN_SECONDS,
              ROUNDING_OF_A_PLACE);
}

/**
 * Given the planet, which moved 1.8 pixels after one second
 * When where it lies is asked for
 * Then it is painted one column to the left, and lies 0.8 pixel left of it
 */
TEST_F(LayeredBackgroundFixture, ASlowLayerLiesAFractionLeftOfItsPixels) {
  LayeredBackground sky = smallSky();

  sky.advance(ONE_SECOND_OF_LAYER, BackgroundPace{});

  EXPECT_EQ(sky.frameOf(PLANET_LAYER_OF_THE_SKY).column,
            PLANET_AFTER_ONE_SECOND);
  EXPECT_NEAR(sky.shiftOf(PLANET_LAYER_OF_THE_SKY),
              SHIFT_OF_THE_PLANET_AFTER_ONE_SECOND, ROUNDING_OF_A_SHIFT);
}

/**
 * Given the near walls of the Pit, which follow the whole scroll
 * When the level has scrolled ten pixels and a half
 * Then they are painted ten columns in, and lie on their pixels
 */
TEST_F(LayeredBackgroundFixture, ALayerThatFollowsTheWholeScrollKeepsTheGrid) {
  LayeredBackground pit = shipped(THE_PIT);

  pit.advance(ONE_FRAME_OF_LAYER, TEN_PIXELS_AND_A_HALF_IN);

  EXPECT_EQ(pit.frameOf(NEAR_WALLS).column, ENTRANCE_TEN_PIXELS_IN);
  EXPECT_FLOAT_EQ(pit.shiftOf(NEAR_WALLS), 0.0F);
}

/**
 * Given the far stars, which warp
 * When they move at one and a half times their speed
 * Then they leave no streak
 */
TEST_F(LayeredBackgroundFixture, LeavesNoStreakUpToOneAndAHalfTimesTheSpeed) {
  LayeredBackground sky = smallSky();

  sky.advance(ONE_FRAME_OF_LAYER, AT_ONE_AND_A_HALF_TIMES_THE_SPEED);

  EXPECT_EQ(sky.frameOf(STAR_LAYER).streak, 0);
}

/**
 * Given the far stars, which warp, and the planet, which does not
 * When they move at forty times their speed
 * Then the stars leave the 3 pixels they cover in a tick, and the planet
 * none
 */
TEST_F(LayeredBackgroundFixture, AWarpStreaksTheLayersThatWarp) {
  LayeredBackground sky = smallSky();

  sky.advance(ONE_FRAME_OF_LAYER, AT_FORTY_TIMES_THE_SPEED);

  EXPECT_EQ(sky.frameOf(STAR_LAYER).streak, STREAK_AT_FORTY_TIMES_THE_SPEED);
  EXPECT_EQ(sky.frameOf(PLANET_LAYER_OF_THE_SKY).streak, 0);
}

/**
 * Given the Pit when it begins, whose near walls leave their 112 first
 * columns empty
 * When where it begins to show is asked for
 * Then it is 112 columns right of the right edge, and its entrance is its
 * last layer
 */
TEST_F(LayeredBackgroundFixture, BeginsToShowWhereItsEntranceHasItsFirstTiles) {
  const LayeredBackground pit = shipped(THE_PIT);

  EXPECT_THAT(pit.entranceLayer(), Optional(NEAR_WALLS));
  EXPECT_THAT(pit.entranceColumn(), Optional(WHERE_THE_PIT_BEGINS_TO_SHOW));
}

/**
 * Given a sky no layer of which is an entrance
 * When its entrance is asked for
 * Then it has none
 */
TEST_F(LayeredBackgroundFixture, ABackgroundThatShowsAtOnceHasNoEntrance) {
  const LayeredBackground sky = smallSky();

  EXPECT_FALSE(sky.entranceLayer().has_value());
  EXPECT_FALSE(sky.entranceColumn().has_value());
}

/**
 * Given the planet at its start
 * When a second passes backwards
 * Then its layer has not moved
 */
TEST_F(LayeredBackgroundFixture, ANegativeTimeMovesNothing) {
  LayeredBackground sky = smallSky();

  sky.advance(ONE_SECOND_BACKWARDS, BackgroundPace{});

  EXPECT_EQ(sky.frameOf(PLANET_LAYER_OF_THE_SKY).column, 0);
}

/**
 * Given the sky the repository ships, whose events are born first after
 * 600 ms
 * When 550 ms pass, then 100 more, a frame of 50 ms at a time
 * Then its event layers paint nothing after the first, and something after
 * the second
 */
TEST_F(LayeredBackgroundFixture, GivesBirthThroughItsGroups) {
  LayeredBackground sky = shipped(THE_SKY);
  ASSERT_EQ(sky.layerCount(), LAYERS_OF_THE_SHIPPED_SKY);

  for (int frame = 0; frame < FRAMES_BEFORE_THE_FIRST_EVENT; ++frame) {
    sky.advance(ONE_FRAME_OF_LAYER, BackgroundPace{});
  }
  const std::size_t beforeTheFirstEvent = paintedFrom(sky, FIRST_EVENT_LAYER);
  for (int frame = 0; frame < FRAMES_UNTIL_THE_FIRST_EVENT; ++frame) {
    sky.advance(ONE_FRAME_OF_LAYER, BackgroundPace{});
  }

  EXPECT_EQ(beforeTheFirstEvent, 0U);
  EXPECT_GT(paintedFrom(sky, FIRST_EVENT_LAYER), 0U);
}

/**
 * Given a background built from the description of a sky
 * When it is asked whether it shows that description, then that of the Pit
 * Then it does the first, and not the second
 */
TEST_F(LayeredBackgroundFixture, KnowsTheDescriptionItShows) {
  const LayeredBackground sky = smallSky();

  EXPECT_TRUE(sky.shows(smallSkyDescription()));
  EXPECT_FALSE(sky.shows(pitDescription()));
}
