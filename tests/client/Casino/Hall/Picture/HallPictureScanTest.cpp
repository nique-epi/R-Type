#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <algorithm>
#include "CasinoHallConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "ColoredPixel.hpp"
#include "HallPicture.hpp"
#include "HallPictureScan.hpp"
#include "HallPictureScanTestConstants.hpp"
#include "SheetPixels.hpp"
#include "Spectator.hpp"

using rtype::client::ColoredPixel;
using rtype::client::HALL_FRONT_ROPE_ROWS;
using rtype::client::HALL_PICTURE_ID;
using rtype::client::HallPicture;
using rtype::client::hallPictureOf;
using rtype::client::Spectator;
using rtype::client::testing::FEWEST_SPECTATORS;
using rtype::client::testing::TOO_SMALL_A_PICTURE;
using ::testing::Each;
using ::testing::Ge;
using ::testing::IsEmpty;
using ::testing::Not;

/**
 * Given the picture of the hall shipped in assets/
 * When the hall reads it
 * Then it finds bulbs to chase
 */
TEST(HallPictureScan, FindsTheBulbsOfTheHall) {
  const HallPicture hall = hallPictureOf(shippedSheet(HALL_PICTURE_ID));

  EXPECT_GT(hall.bulbs.size(), 0U);
}

/**
 * Given the picture of the hall shipped in assets/
 * When the hall reads it
 * Then it keeps pixels of the front ropes, each on a row of a rope
 */
TEST(HallPictureScan, KeepsTheFrontRopesOnTheirRows) {
  const HallPicture hall = hallPictureOf(shippedSheet(HALL_PICTURE_ID));

  ASSERT_THAT(hall.frontRopes, Not(IsEmpty()));
  for (const ColoredPixel& rope : hall.frontRopes) {
    EXPECT_NE(std::ranges::find(HALL_FRONT_ROPE_ROWS, rope.position.y),
              HALL_FRONT_ROPE_ROWS.end());
  }
}

/**
 * Given the picture of the hall shipped in assets/
 * When the hall reads its crowd
 * Then it cuts it into spectators who hop on their own
 */
TEST(HallPictureScan, CutsTheCrowdIntoSpectators) {
  const HallPicture hall = hallPictureOf(shippedSheet(HALL_PICTURE_ID));

  EXPECT_THAT(hall.spectators.size(), Ge(FEWEST_SPECTATORS));
}

/**
 * Given a picture smaller than the interface
 * When the hall reads it
 * Then it finds nothing, and reads no pixel outside the picture
 */
TEST(HallPictureScan, APictureOfAnotherSizeGivesAStillHall) {
  const HallPicture hall = hallPictureOf(sf::Image(TOO_SMALL_A_PICTURE));

  EXPECT_EQ(hall.bulbs.size(), 0U);
  EXPECT_THAT(hall.frontRopes, IsEmpty());
  EXPECT_THAT(hall.spectators, IsEmpty());
}
