#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include "BackgroundCatalog.hpp"
#include "LayeredBackground.hpp"
#include "LayeredBackgroundFixture.hpp"
#include "ShownBackgrounds.hpp"
#include "ShownBackgroundsFixture.hpp"
#include "ShownBackgroundsTestConstants.hpp"
#include "SkyLevel.hpp"

using rtype::client::BackgroundCatalog;
using rtype::client::ShownBackgrounds;
using rtype::client::SkyLevel;

/**
 * Given the backgrounds the repository ships
 * When no level was shown yet
 * Then the sky shows the default background, and none is leaving
 */
TEST_F(ShownBackgroundsFixture, ShowsTheDefaultBackgroundBeforeAnyLevel) {
  const ShownBackgrounds backgrounds = shippedSky();

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_TRUE(backgrounds.shown()->shows(described(SHIPPED_SKY)));
  EXPECT_EQ(backgrounds.leaving(), nullptr);
}

/**
 * Given the sky, shown for one second: its planet drifted 1.8 pixels
 * When a level without a biome is shown
 * Then the sky goes on from where it was, its planet still 1.8 pixels left
 */
TEST_F(ShownBackgroundsFixture, ALevelWithoutABiomeKeepsTheSkyGoing) {
  ShownBackgrounds backgrounds = shippedSky();
  passOneSecond(backgrounds);

  backgrounds.show(SkyLevel{});

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_NEAR(whereLies(*backgrounds.shown(), PLANET_OF_THE_SKY),
              PLANET_AFTER_ONE_SECOND_OF_SKY, ROUNDING_OF_THE_SKY);
}

/**
 * Given the sky
 * When a level in the biome of the Pit is shown
 * Then the background shown is the Pit, and the sky is leaving behind it
 */
TEST_F(ShownBackgroundsFixture, ABackgroundArrivesOverTheOneItReplaces) {
  ShownBackgrounds backgrounds = shippedSky();

  backgrounds.show(levelInThePit(0.0F));

  ASSERT_NE(backgrounds.shown(), nullptr);
  ASSERT_NE(backgrounds.leaving(), nullptr);
  EXPECT_TRUE(backgrounds.shown()->shows(described(SHIPPED_PIT)));
  EXPECT_TRUE(backgrounds.leaving()->shows(described(SHIPPED_SKY)));
}

/**
 * Given the sky leaving behind the Pit, which has just begun
 * When one second passes
 * Then the planet of the sky that leaves drifted its 1.8 pixels
 */
TEST_F(ShownBackgroundsFixture, TheBackgroundThatLeavesKeepsMoving) {
  ShownBackgrounds backgrounds = shippedSky();
  backgrounds.show(levelInThePit(0.0F));

  passOneSecond(backgrounds);

  ASSERT_NE(backgrounds.leaving(), nullptr);
  EXPECT_NEAR(whereLies(*backgrounds.leaving(), PLANET_OF_THE_SKY),
              PLANET_AFTER_ONE_SECOND_OF_SKY, ROUNDING_OF_THE_SKY);
}

/**
 * Given the Pit 400 units in: its entrance stands on the column 492
 * When a frame passes
 * Then the sky is still leaving behind it
 */
TEST_F(ShownBackgroundsFixture, KeepsTheOneThatLeavesWhileTheEntranceShows) {
  ShownBackgrounds backgrounds = shippedSky();

  backgrounds.show(levelInThePit(FOUR_HUNDRED_UNITS));
  backgrounds.advance(ONE_FRAME_OF_SKY);

  EXPECT_NE(backgrounds.leaving(), nullptr);
}

/**
 * Given the Pit 2600 units in: its entrance stands on the column -58, out of
 * sight with its fringe
 * When a frame passes
 * Then no background is leaving any more
 */
TEST_F(ShownBackgroundsFixture, DropsTheOneThatLeftOnceTheEntranceIsGone) {
  ShownBackgrounds backgrounds = shippedSky();

  backgrounds.show(levelInThePit(TWO_THOUSAND_SIX_HUNDRED_UNITS));
  backgrounds.advance(ONE_FRAME_OF_SKY);

  EXPECT_EQ(backgrounds.leaving(), nullptr);
}

/**
 * Given the sky
 * When a level in a biome no background is named after is shown
 * Then the sky stays, and none is leaving
 */
TEST_F(ShownBackgroundsFixture, ABiomeWithoutABackgroundShowsTheDefault) {
  ShownBackgrounds backgrounds = shippedSky();

  backgrounds.show(SkyLevel{.biome = std::string{BIOME_WITHOUT_A_BACKGROUND}});

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_TRUE(backgrounds.shown()->shows(described(SHIPPED_SKY)));
  EXPECT_EQ(backgrounds.leaving(), nullptr);
}

/**
 * Given the Pit, with the sky leaving behind it
 * When a level without a biome is shown
 * Then the sky, which has no entrance, replaces the Pit at once
 */
TEST_F(ShownBackgroundsFixture, ABackgroundWithoutAnEntranceReplacesAtOnce) {
  ShownBackgrounds backgrounds = shippedSky();
  backgrounds.show(levelInThePit(0.0F));

  backgrounds.show(SkyLevel{});

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_TRUE(backgrounds.shown()->shows(described(SHIPPED_SKY)));
  EXPECT_EQ(backgrounds.leaving(), nullptr);
}

/**
 * Given the Pit shown 100 units in, by a level that scrolls 120 units per
 * second
 * When one second passes without another message
 * Then its far walls followed the scroll to 220 units: 13.75 pixels left
 */
TEST_F(ShownBackgroundsFixture, FollowsTheScrollBetweenTwoMessages) {
  ShownBackgrounds backgrounds = shippedSky();
  SkyLevel level = levelInThePit(A_HUNDRED_UNITS);
  level.scrollSpeed = A_HUNDRED_AND_TWENTY_UNITS_PER_SECOND;
  backgrounds.show(level);

  passOneSecond(backgrounds);

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_NEAR(whereLies(*backgrounds.shown(), FAR_WALLS_OF_THE_PIT),
              FAR_WALLS_ONE_SECOND_LATER, ROUNDING_OF_THE_SKY);
}

/**
 * Given the sky
 * When ten seconds pass at once, as after a stall
 * Then its planet moved by 50 ms only, the longest step: 0.09 pixel
 */
TEST_F(ShownBackgroundsFixture, AStallMovesNoFurtherThanTheLongestStep) {
  ShownBackgrounds backgrounds = shippedSky();

  backgrounds.advance(A_STALL);

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_NEAR(whereLies(*backgrounds.shown(), PLANET_OF_THE_SKY),
              PLANET_AFTER_A_STALL, ROUNDING_OF_THE_SKY);
}

/**
 * Given the Pit shown 400 units in, its entrance on the column 380, then
 * left for the sky
 * When a level in the Pit that has just begun is shown again
 * Then the Pit starts over: its entrance is back on the right edge
 */
TEST_F(ShownBackgroundsFixture, ABackgroundShownAgainStartsOver) {
  ShownBackgrounds backgrounds = shippedSky();
  backgrounds.show(levelInThePit(FOUR_HUNDRED_UNITS));
  backgrounds.advance(ONE_FRAME_OF_SKY);
  ASSERT_NE(backgrounds.shown(), nullptr);
  const int whenItLeft =
      backgrounds.shown()->frameOf(NEAR_WALLS_OF_THE_PIT).column;
  backgrounds.show(SkyLevel{});

  backgrounds.show(levelInThePit(0.0F));
  backgrounds.advance(ONE_FRAME_OF_SKY);

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_EQ(whenItLeft, ENTRANCE_FOUR_HUNDRED_UNITS_IN);
  EXPECT_EQ(backgrounds.shown()->frameOf(NEAR_WALLS_OF_THE_PIT).column,
            RIGHT_EDGE_OF_THE_SKY);
}

/**
 * Given the sky, whose far stars warp
 * When the speed factor is set to forty, and a frame passes
 * Then the far stars leave their streak of 3 pixels
 */
TEST_F(ShownBackgroundsFixture, WarpsTheLayersOfTheBackgroundShown) {
  ShownBackgrounds backgrounds = shippedSky();

  backgrounds.setSpeedFactor(FORTY_TIMES_THE_SPEED);
  backgrounds.advance(ONE_FRAME_OF_SKY);

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_EQ(backgrounds.shown()->frameOf(FAR_STARS_OF_THE_SKY).streak,
            STREAK_OF_THE_FAR_STARS);
}

/**
 * Given the sky, whose far stars warp
 * When the speed factor is set below zero, and one second passes
 * Then the far stars have not moved: the factor counts as zero
 */
TEST_F(ShownBackgroundsFixture, ASpeedFactorBelowZeroStopsTheLayersThatWarp) {
  ShownBackgrounds backgrounds = shippedSky();

  backgrounds.setSpeedFactor(A_SPEED_FACTOR_BELOW_ZERO);
  passOneSecond(backgrounds);

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_FLOAT_EQ(whereLies(*backgrounds.shown(), FAR_STARS_OF_THE_SKY), 0.0F);
}

/**
 * Given the sky, and the pack of a server whose space is a sky of one planet
 * When the backgrounds of the pack are laid over
 * Then the background shown is the space of the pack
 */
TEST_F(ShownBackgroundsFixture, ABackgroundLaidOverReplacesTheOneOfItsName) {
  ShownBackgrounds backgrounds = shippedSky();
  const std::shared_ptr<const BackgroundCatalog> pack =
      packOf(SPACE_OF_A_PACK, A_SKY_OF_THE_SHIPPED_PLANET);
  ASSERT_NE(pack->named(SHIPPED_SKY), nullptr);

  backgrounds.layOver(pack);

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_TRUE(backgrounds.shown()->shows(*pack->named(SHIPPED_SKY)));
}

/**
 * Given the sky, with the pack of a server that brings a nebula laid over
 * When a level in the biome of the nebula is shown
 * Then the background shown is the nebula of the pack
 */
TEST_F(ShownBackgroundsFixture, ABackgroundLaidOverIsShownForItsBiome) {
  ShownBackgrounds backgrounds = shippedSky();
  const std::shared_ptr<const BackgroundCatalog> pack =
      packOf(NEBULA_OF_A_PACK, A_SKY_OF_THE_SHIPPED_PLANET);
  ASSERT_NE(pack->named(NEBULA_OF_THE_PACK), nullptr);
  backgrounds.layOver(pack);

  backgrounds.show(SkyLevel{.biome = std::string{NEBULA_OF_THE_PACK}});

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_TRUE(backgrounds.shown()->shows(*pack->named(NEBULA_OF_THE_PACK)));
}

/**
 * Given the space of the pack of a server laid over the sky
 * When what is laid over is taken off
 * Then the background shown is the sky of the client again
 */
TEST_F(ShownBackgroundsFixture, TakingOffWhatIsLaidOverShowsTheSkyAgain) {
  ShownBackgrounds backgrounds = shippedSky();
  backgrounds.layOver(packOf(SPACE_OF_A_PACK, A_SKY_OF_THE_SHIPPED_PLANET));

  backgrounds.layOver(nullptr);

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_TRUE(backgrounds.shown()->shows(described(SHIPPED_SKY)));
}

/**
 * Given the sky, shown for one second: its planet drifted 1.8 pixels
 * When the pack of a server that brings only a nebula is laid over
 * Then the sky goes on from where it was, its planet still 1.8 pixels left
 */
TEST_F(ShownBackgroundsFixture, LayingOverKeepsTheBackgroundItLeavesGoing) {
  ShownBackgrounds backgrounds = shippedSky();
  passOneSecond(backgrounds);

  backgrounds.layOver(packOf(NEBULA_OF_A_PACK, A_SKY_OF_THE_SHIPPED_PLANET));

  ASSERT_NE(backgrounds.shown(), nullptr);
  EXPECT_NEAR(whereLies(*backgrounds.shown(), PLANET_OF_THE_SKY),
              PLANET_AFTER_ONE_SECOND_OF_SKY, ROUNDING_OF_THE_SKY);
}

/**
 * Given the Pit arriving over the sky, which is leaving behind it
 * When the pack of a server whose space replaces the sky is laid over
 * Then no background is leaving: the one that was is built anew
 */
TEST_F(ShownBackgroundsFixture, LayingOverLeavesNoBackgroundBehind) {
  ShownBackgrounds backgrounds = shippedSky();
  backgrounds.show(levelInThePit(0.0F));
  ASSERT_NE(backgrounds.leaving(), nullptr);

  backgrounds.layOver(packOf(SPACE_OF_A_PACK, A_SKY_OF_THE_SHIPPED_PLANET));

  EXPECT_EQ(backgrounds.leaving(), nullptr);
}

/**
 * Given a folder that holds no background
 * When its backgrounds are shown, a level is shown and time passes
 * Then nothing is shown, and nothing is thrown
 */
TEST(ShownBackgrounds, ACatalogWithoutBackgroundsShowsNothing) {
  const std::array folders{std::filesystem::path{FOLDER_WITHOUT_BACKGROUNDS}};
  const BackgroundCatalog none = BackgroundCatalog::ofLastFolder(folders);
  ShownBackgrounds backgrounds{none, shippedSources()};

  backgrounds.show(levelInThePit(0.0F));
  backgrounds.advance(ONE_FRAME_OF_SKY);

  EXPECT_EQ(backgrounds.shown(), nullptr);
  EXPECT_EQ(backgrounds.leaving(), nullptr);
}
