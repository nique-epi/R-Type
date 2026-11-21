#include <gtest/gtest.h>
#include <optional>
#include "ContinueBanner.hpp"
#include "ContinueBannerTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "StatusBandTestConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::ContinueBanner;
using rtype::client::testing::WARNING;

/**
 * Given a banner that heard nothing of the server
 * When the first status of the level tells 3 continues
 * Then it shows nothing: no continue was taken
 */
TEST(ContinueBanner, TheFirstCountToldShowsNothing) {
  ContinueBanner banner;

  banner.follow(THREE_CONTINUES_TOLD);

  EXPECT_EQ(banner.shownContinues(), std::nullopt);
}

/**
 * Given a banner told 3 continues
 * When a status tells 2
 * Then it shows, telling the 2 left
 */
TEST(ContinueBanner, ShowsOnceAContinueIsTaken) {
  ContinueBanner banner;
  banner.follow(THREE_CONTINUES_TOLD);

  banner.follow(TWO_CONTINUES_TOLD);

  EXPECT_EQ(banner.shownContinues(), std::optional{TWO_CONTINUES_TOLD});
}

/**
 * Given a banner told 3 continues
 * When a status tells 3 again, as a change of biome does
 * Then it shows nothing
 */
TEST(ContinueBanner, ShowsNothingWhileTheContinuesStay) {
  ContinueBanner banner;
  banner.follow(THREE_CONTINUES_TOLD);

  banner.follow(THREE_CONTINUES_TOLD);

  EXPECT_EQ(banner.shownContinues(), std::nullopt);
}

/**
 * Given a banner shown
 * When one nanosecond less than 2 s passes
 * Then it still shows
 */
TEST(ContinueBanner, StillShowsJustUnderTwoSeconds) {
  ContinueBanner banner;
  banner.follow(THREE_CONTINUES_TOLD);
  banner.follow(TWO_CONTINUES_TOLD);

  banner.advance(TIME_OF_THE_CONTINUE_BANNER - ONE_NANOSECOND);

  EXPECT_EQ(banner.shownContinues(), std::optional{TWO_CONTINUES_TOLD});
}

/**
 * Given a banner shown
 * When 2 s pass
 * Then it is gone
 */
TEST(ContinueBanner, IsGoneAfterTwoSeconds) {
  ContinueBanner banner;
  banner.follow(THREE_CONTINUES_TOLD);
  banner.follow(TWO_CONTINUES_TOLD);

  banner.advance(TIME_OF_THE_CONTINUE_BANNER);

  EXPECT_EQ(banner.shownContinues(), std::nullopt);
}

/**
 * Given a banner shown for 1.5 s
 * When another continue is taken and 1.5 s more pass
 * Then it still shows, telling the 1 left: its time started over
 */
TEST(ContinueBanner, AContinueTakenWhileItShowsStartsItsTimeOver) {
  ContinueBanner banner;
  banner.follow(THREE_CONTINUES_TOLD);
  banner.follow(TWO_CONTINUES_TOLD);
  banner.advance(MOST_OF_THE_CONTINUE_BANNER);

  banner.follow(ONE_CONTINUE_TOLD);
  banner.advance(MOST_OF_THE_CONTINUE_BANNER);

  EXPECT_EQ(banner.shownContinues(), std::optional{ONE_CONTINUE_TOLD});
}

/**
 * Given a banner telling 2 continues left
 * When it is painted
 * Then CONTINUE and 2 LEFT are written between two rules
 */
TEST(ContinueBanner, WritesTheContinuesLeftUnderItsTitle) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  ContinueBanner banner;
  banner.follow(THREE_CONTINUES_TOLD);
  banner.follow(TWO_CONTINUES_TOLD);

  banner.paint(canvas);

  EXPECT_NE(writingOf(record, CONTINUE_TITLE_ON_SCREEN), nullptr);
  EXPECT_NE(writingOf(record, TWO_LEFT_ON_SCREEN), nullptr);
  EXPECT_EQ(record.fills.size(), RULES_OF_THE_CONTINUE_BANNER);
}

/**
 * Given a banner whose team took its last continue
 * When it is painted
 * Then NO CONTINUE LEFT is written in the orange of the warnings
 */
TEST(ContinueBanner, WarnsOnceNoContinueIsLeft) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  ContinueBanner banner;
  banner.follow(ONE_CONTINUE_TOLD);
  banner.follow(NO_CONTINUE_TOLD);

  banner.paint(canvas);

  const RecordedWriting* const line = writingOf(record, NONE_LEFT_ON_SCREEN);
  ASSERT_NE(line, nullptr);
  EXPECT_EQ(line->style.color, WARNING);
}

/**
 * Given a banner that does not show
 * When it is painted
 * Then nothing is written or filled
 */
TEST(ContinueBanner, PaintsNothingWhileItDoesNotShow) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  ContinueBanner banner;
  banner.follow(THREE_CONTINUES_TOLD);

  banner.paint(canvas);

  EXPECT_TRUE(record.writings.empty());
  EXPECT_TRUE(record.fills.empty());
}
