#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include "ImageLayer.hpp"
#include "ImageLayerFixtures.hpp"
#include "ImageLayerTestConstants.hpp"
#include "LayerTestConstants.hpp"
#include "RecordedFrame.hpp"

using rtype::client::ImageLayer;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given the planet in a layer that never repeats, at its origin
 * When it is painted
 * Then its image is drawn once, whole, with its corner on (288, 43)
 */
TEST(ImageLayer, PaintsItsImageWhereItsDescriptionPutsIt) {
  const ImageLayer planet = planetLayer(NO_REPEAT);

  const std::vector<RecordedFrame> frames =
      framesPaintedBy(planet, AT_ITS_ORIGIN);

  EXPECT_THAT(frames,
              ElementsAre(RecordedFrame{.file = std::string{PLANET_IMAGE},
                                        .frames = ONE_FRAME_STRIP,
                                        .frame = WHOLE_IMAGE,
                                        .topLeft = PLANET_PIXEL}));
}

/**
 * Given the planet in a layer that moved 10 columns to the left
 * When it is painted
 * Then its image is drawn 10 columns left of its place
 */
TEST(ImageLayer, MovesWithItsLayer) {
  const ImageLayer planet = planetLayer(NO_REPEAT);

  const std::vector<RecordedFrame> frames =
      framesPaintedBy(planet, TEN_COLUMNS_LEFT);

  EXPECT_THAT(cornersOnTheBackground(frames),
              ElementsAre(PLANET_TEN_COLUMNS_LEFT));
}

/**
 * Given the planet in a layer that repeats every 647 columns, moved 445
 * columns: its image has just left on the left, the next copy lies on 490
 * When it is painted
 * Then no copy has its corner on the background
 */
TEST(ImageLayer, ShowsNoCopyBetweenTheOneThatLeftAndTheNext) {
  const ImageLayer planet = planetLayer(PLANET_LAYER_REPEAT);

  const std::vector<RecordedFrame> frames =
      framesPaintedBy(planet, PLANET_JUST_GONE);

  EXPECT_THAT(cornersOnTheBackground(frames), IsEmpty());
}

/**
 * Given the planet in a layer that repeats every 647 columns, moved 456
 * columns
 * When it is painted
 * Then the next copy comes in from the right, its corner on the last column
 */
TEST(ImageLayer, BringsTheNextCopyInFromTheRight) {
  const ImageLayer planet = planetLayer(PLANET_LAYER_REPEAT);

  const std::vector<RecordedFrame> frames =
      framesPaintedBy(planet, NEXT_PLANET_ENTERING);

  EXPECT_THAT(cornersOnTheBackground(frames),
              ElementsAre(NEXT_PLANET_ON_THE_LAST_COLUMN));
}

/**
 * Given the planet, whose image lies on the column 288 of its layer
 * When where it begins is asked for
 * Then it is that column
 */
TEST(ImageLayer, BeginsOnTheColumnOfItsImage) {
  const ImageLayer planet = planetLayer(NO_REPEAT);

  EXPECT_EQ(planet.leftEdge(), LEFT_EDGE_OF_THE_PLANET);
}
