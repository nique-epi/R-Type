#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include "BossBar.hpp"
#include "BossBarFixtures.hpp"
#include "BossBarLook.hpp"
#include "BossBarTestConstants.hpp"
#include "BossFight.hpp"
#include "BossGaugeState.hpp"
#include "BossStatus.hpp"
#include "FlashPreference.hpp"
#include "GameNoticeConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextStyle.hpp"

using rtype::client::bossBarAt;
using rtype::client::BossBarLook;
using rtype::client::BossFight;
using rtype::client::BossGaugeState;
using rtype::client::FlashPreference;
using rtype::client::SILENCE_WARNING_GAP;
using rtype::client::TextAlignment;
using rtype::client::WEAK_SIGNAL_TOP_LEFT;
using rtype::network::BossStage;
using ::testing::Contains;
using ::testing::Field;
using ::testing::IsEmpty;

/**
 * Given no boss in its fight yet
 * When the bar is read
 * Then nothing shows
 */
TEST(BossBar, NothingShowsBeforeABossFights) {
  EXPECT_EQ(bossBarAt(BossFight{}, A_HIT_AT_ONE_SECOND, FlashPreference::Full),
            BossBarLook{});
}

/**
 * Given a boss with all its points, fighting from 0
 * When the bar is read at 0, then at 800 ms
 * Then its gauges light nothing, then every column
 */
TEST(BossBar, FillsFromNothingInEightHundredMilliseconds) {
  const BossFight fight = fightWith(BossStage::Fighting, BAR_FULL_HEALTH);

  const BossBarLook first =
      bossBarAt(fight, FIGHT_START, FlashPreference::Full);
  const BossBarLook filled =
      bossBarAt(fight, BAR_FILLED, FlashPreference::Full);

  EXPECT_EQ(first.phaseOne.fill, 0);
  EXPECT_EQ(first.phaseTwo.fill, 0);
  EXPECT_EQ(filled.phaseOne.fill, GAUGE_INNER_COLUMNS);
  EXPECT_EQ(filled.phaseTwo.fill, GAUGE_INNER_COLUMNS);
}

/**
 * Given a boss hit from 240 to 230 points at one second
 * When the bar is read 100 ms later
 * Then gauge I lights 90 columns, its lag running on to the 98 of 240 points
 */
TEST(BossBar, ShowsThePointsJustLostAsALag) {
  BossFight fight = fightWith(BossStage::Fighting, BAR_AFTER_ONE_HIT);
  fight.lagHealth = BAR_FULL_HEALTH;
  fight.lastLossAt = A_HIT_AT_ONE_SECOND;

  const BossBarLook look =
      bossBarAt(fight, JUST_AFTER_THE_HIT, FlashPreference::Full);

  EXPECT_EQ(look.phaseOne.fill, COLUMNS_OF_110_POINTS_OF_120);
  EXPECT_EQ(look.phaseOne.lagEnd, GAUGE_INNER_COLUMNS);
}

/**
 * Given a boss hit from 240 to 230 points at one second
 * When the bar is read half a second later
 * Then the lag is gone: it ends where the fill does
 */
TEST(BossBar, TheLagGoesAfterHalfASecond) {
  BossFight fight = fightWith(BossStage::Fighting, BAR_AFTER_ONE_HIT);
  fight.lagHealth = BAR_FULL_HEALTH;
  fight.lastLossAt = A_HIT_AT_ONE_SECOND;

  const BossBarLook look =
      bossBarAt(fight, HALF_A_SECOND_AFTER_THE_HIT, FlashPreference::Full);

  EXPECT_EQ(look.phaseOne.lagEnd, COLUMNS_OF_110_POINTS_OF_120);
}

/**
 * Given an enraged boss with 119 of its 240 points
 * When the bar is read
 * Then gauge I is spent and gauge II enraged
 */
TEST(BossBar, TheRageSpendsGaugeOneAndEnragesGaugeTwo) {
  const BossBarLook look =
      bossBarAt(fightWith(BossStage::Enraged, BAR_BELOW_HALF), BAR_FILLED,
                FlashPreference::Full);

  EXPECT_EQ(look.phaseOne.state, BossGaugeState::Spent);
  EXPECT_EQ(look.phaseTwo.state, BossGaugeState::Enraged);
}

/**
 * Given the bar of a boss in its fight
 * When it is painted
 * Then its two gauges are 100 by 5 from y 16, at x 139 and 242: the bar runs
 * from x 139 to 341, at the top centre
 */
TEST(BossBar, PaintsTheTwoGaugesAtTheTopCentre) {
  const InterfaceCanvasRecord record =
      barPaintingOf(bossBarAt(fightWith(BossStage::Fighting, BAR_FULL_HEALTH),
                              BAR_FILLED, FlashPreference::Full));

  EXPECT_THAT(record.fills,
              Contains(Field(&RecordedFill::area, DESIGNED_PHASE_TWO_GAUGE)));
  EXPECT_THAT(record.fills,
              Contains(Field(&RecordedFill::area, DESIGNED_PHASE_ONE_GAUGE)));
}

/**
 * Given an enraged boss
 * When the bar is read at 800 ms, 879 ms, then 880 ms
 * Then its hot pixels are those of step 10, still 10, then 11
 */
TEST(BossBar, TheRageFlickersEveryEightyMilliseconds) {
  const BossFight fight = fightWith(BossStage::Enraged, BAR_BELOW_HALF);

  EXPECT_EQ(bossBarAt(fight, FLICKER_STEP_TEN_START, FlashPreference::Full)
                .flickerStep,
            FLICKER_STEP_TEN);
  EXPECT_EQ(
      bossBarAt(fight, FLICKER_STEP_TEN_END, FlashPreference::Full).flickerStep,
      FLICKER_STEP_TEN);
  EXPECT_EQ(bossBarAt(fight, FLICKER_STEP_ELEVEN_START, FlashPreference::Full)
                .flickerStep,
            FLICKER_STEP_ELEVEN);
}

/**
 * Given an enraged boss, flashes reduced
 * When the bar is read at 880 ms
 * Then its hot pixels stay those of step 0: they never flicker
 */
TEST(BossBar, TheRageDoesNotFlickerWithReducedFlashes) {
  EXPECT_EQ(bossBarAt(fightWith(BossStage::Enraged, BAR_BELOW_HALF),
                      FLICKER_STEP_ELEVEN_START, FlashPreference::Reduced)
                .flickerStep,
            0);
}

/**
 * Given gauge I emptied at two seconds into a fight that started at 0
 * When the bar is read 100 ms later, then 500 ms later
 * Then it is 100 ms into its shatter, then no longer shattering
 */
TEST(BossBar, GaugeOneShattersForHalfASecondOnceEmpty) {
  BossFight fight = fightWith(BossStage::Enraged, BAR_BELOW_HALF);
  fight.phaseOneEmptiedAt = GAUGE_ONE_EMPTIED;

  EXPECT_EQ(bossBarAt(fight, INTO_THE_SHATTER, FlashPreference::Full)
                .shatterMilliseconds,
            std::optional<int>{MILLISECONDS_INTO_THE_SHATTER});
  EXPECT_FALSE(bossBarAt(fight, SHATTER_OVER, FlashPreference::Full)
                   .shatterMilliseconds.has_value());
}

/**
 * Given gauge I already empty on the first status of the fight
 * When the bar is read 100 ms later
 * Then it does not shatter: it never showed full
 */
TEST(BossBar, AGaugeAlreadyEmptyWhenTheBarAppearsDoesNotShatter) {
  BossFight fight = fightWith(BossStage::Enraged, BAR_BELOW_HALF);
  fight.fightingSince = GAUGE_ONE_EMPTIED;
  fight.phaseOneEmptiedAt = GAUGE_ONE_EMPTIED;

  EXPECT_FALSE(bossBarAt(fight, INTO_THE_SHATTER, FlashPreference::Full)
                   .shatterMilliseconds.has_value());
}

/**
 * Given a defeated boss
 * When the bar is read
 * Then nothing shows
 */
TEST(BossBar, NothingShowsOnceTheBossIsDefeated) {
  EXPECT_EQ(bossBarAt(fightWith(BossStage::Defeated, 0), BAR_FILLED,
                      FlashPreference::Full),
            BossBarLook{});
}

/**
 * Given an enraged boss with one point of its 240 left
 * When the bar is read once filled
 * Then gauge II still lights one column
 */
TEST(BossBar, ALastPointStillLightsAColumn) {
  EXPECT_EQ(bossBarAt(fightWith(BossStage::Enraged, BAR_LAST_POINT), BAR_FILLED,
                      FlashPreference::Full)
                .phaseTwo.fill,
            ONE_COLUMN);
}

/**
 * Given the bar of a boss in its fight
 * When it is painted
 * Then BYDO TYRANT is written centred on x 240, its top on y 6, above the bar
 */
TEST(BossBar, TheNameIsCentredAboveTheBar) {
  const InterfaceCanvasRecord record =
      barPaintingOf(bossBarAt(fightWith(BossStage::Fighting, BAR_FULL_HEALTH),
                              BAR_FILLED, FlashPreference::Full));

  const RecordedWriting* const name = writingOf(record, DESIGNED_BOSS_NAME);
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->anchor, DESIGNED_NAME_ANCHOR);
  EXPECT_EQ(name->style.alignment, TextAlignment::Center);
}

/**
 * Given the bar of an enraged boss shattering its gauge I
 * When it is painted
 * Then no pixel of it lies right of where the silence warning ends its text,
 * SILENCE_WARNING_GAP left of the bars of the weak signal
 */
TEST(BossBar, TheBarNeverReachesTheSilenceWarning) {
  BossFight fight = fightWith(BossStage::Enraged, BAR_BELOW_HALF);
  fight.phaseOneEmptiedAt = GAUGE_ONE_EMPTIED;
  const InterfaceCanvasRecord record =
      barPaintingOf(bossBarAt(fight, INTO_THE_SHATTER, FlashPreference::Full));

  for (const auto& [column, row] : litPixelsOf(record)) {
    EXPECT_LT(column, WEAK_SIGNAL_TOP_LEFT.x - SILENCE_WARNING_GAP);
  }
}

/**
 * Given a boss fighting, at the largest text size
 * When its bar is painted
 * Then its name stays on screen, above the gauges
 */
TEST(BossBar, AtTheLargestSizeTheNameStaysOnScreen) {
  const BossFight fight = fightWith(BossStage::Fighting, BAR_FULL_HEALTH);
  const InterfaceCanvasRecord painting = barPaintingOf(
      bossBarAt(fight, BAR_FILLED, FlashPreference::Full), LARGEST_BAR_TEXT);

  EXPECT_THAT(writingsOffScreen(painting), IsEmpty());
}
