#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <span>
#include "CasinoSpriteConstants.hpp"
#include "ColoredPixel.hpp"
#include "EntranceLightConstants.hpp"
#include "EntranceLightScan.hpp"
#include "EntranceLightScanTestConstants.hpp"
#include "EntranceLights.hpp"
#include "SheetPixels.hpp"

using rtype::client::CASINO_DOORS_CLOSED_ID;
using rtype::client::ColoredPixel;
using rtype::client::ENTRANCE_MARQUEE_LETTERS;
using rtype::client::EntranceLights;
using rtype::client::entranceLightsOf;
using rtype::client::testing::A_STEP_OF_THE_CHASE;
using rtype::client::testing::ENTRANCE_TOO_SMALL_A_PICTURE;
using ::testing::IsEmpty;
using ::testing::Not;

/**
 * Given the front of the casino shipped in assets/, its doors closed
 * When the front reads it
 * Then it finds the bulbs of the oval, of the columns and of the marquee
 */
TEST(EntranceLightScan, FindsTheBulbsOfThePylonAndOfTheMarquee) {
  const EntranceLights lights =
      entranceLightsOf(shippedSheet(CASINO_DOORS_CLOSED_ID));

  EXPECT_GT(lights.oval.size(), 0U);
  EXPECT_GT(lights.columns.size(), 0U);
  EXPECT_GT(lights.marquee.size(), 0U);
}

/**
 * Given the front of the casino shipped in assets/
 * When the front reads its marquee
 * Then no bulb it chases lies on the letters, which stay lit to stay
 * readable
 */
TEST(EntranceLightScan, LeavesTheLettersOfTheMarqueeLit) {
  const EntranceLights lights =
      entranceLightsOf(shippedSheet(CASINO_DOORS_CLOSED_ID));

  for (int step = 0; step < A_STEP_OF_THE_CHASE; ++step) {
    for (const ColoredPixel& bulb : lights.marquee.unlitAt(step)) {
      EXPECT_FALSE(ENTRANCE_MARQUEE_LETTERS.contains(bulb.position));
    }
  }
}

/**
 * Given the front of the casino shipped in assets/
 * When the front reads its pylon
 * Then it finds rows of golden discs and a neon script to cut out
 */
TEST(EntranceLightScan, FindsTheDiscsAndTheScriptOfThePylon) {
  const EntranceLights lights =
      entranceLightsOf(shippedSheet(CASINO_DOORS_CLOSED_ID));

  EXPECT_THAT(lights.litDiscRows, Not(IsEmpty()));
  EXPECT_THAT(lights.darkScript, Not(IsEmpty()));
}

/**
 * Given a picture smaller than the interface
 * When the front reads it
 * Then it finds nothing, and reads no pixel outside the picture
 */
TEST(EntranceLightScan, APictureOfAnotherSizeGivesAStillFront) {
  const EntranceLights lights =
      entranceLightsOf(sf::Image(ENTRANCE_TOO_SMALL_A_PICTURE));

  EXPECT_EQ(lights.oval.size(), 0U);
  EXPECT_EQ(lights.marquee.size(), 0U);
  EXPECT_THAT(lights.stars, IsEmpty());
}
