#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "ContinueNotice.hpp"
#include "ContinueNoticeTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::ContinueNotice;
using rtype::client::continueNoticeText;

/**
 * Given a notice that never had a status
 * When the first status says the team holds all 3 continues
 * Then it shows nothing: no continue was spent
 */
TEST(ContinueNotice, TheFirstStatusShowsNothing) {
  ContinueNotice notice;

  notice.follow(EVERY_CONTINUE, FIRST_STATUS_AT);

  EXPECT_EQ(notice.shownAt(FIRST_STATUS_AT), std::nullopt);
}

/**
 * Given a team that held 3 continues
 * When a status says it holds 2
 * Then the notice shows 2 continues left
 */
TEST(ContinueNotice, ASpentContinueShowsTheContinuesLeft) {
  ContinueNotice notice;
  notice.follow(EVERY_CONTINUE, FIRST_STATUS_AT);

  notice.follow(TWO_CONTINUES, CONTINUE_SPENT_AT);

  EXPECT_EQ(notice.shownAt(CONTINUE_SPENT_AT),
            std::optional<std::uint8_t>{TWO_CONTINUES});
}

/**
 * Given a continue spent
 * When 2 seconds less a nanosecond pass
 * Then the notice still shows
 */
TEST(ContinueNotice, ShowsForTwoSeconds) {
  ContinueNotice notice;
  notice.follow(EVERY_CONTINUE, FIRST_STATUS_AT);
  notice.follow(TWO_CONTINUES, CONTINUE_SPENT_AT);

  EXPECT_NE(notice.shownAt(CONTINUE_SPENT_AT + CONTINUE_NOTICE_SHOWN -
                           ONE_NANOSECOND),
            std::nullopt);
}

/**
 * Given a continue spent
 * When 2 seconds pass
 * Then the notice is gone
 */
TEST(ContinueNotice, IsGoneAfterTwoSeconds) {
  ContinueNotice notice;
  notice.follow(EVERY_CONTINUE, FIRST_STATUS_AT);
  notice.follow(TWO_CONTINUES, CONTINUE_SPENT_AT);

  EXPECT_EQ(notice.shownAt(CONTINUE_SPENT_AT + CONTINUE_NOTICE_SHOWN),
            std::nullopt);
}

/**
 * Given a team that held 2 continues
 * When another status says it still holds 2, as after a change of biome
 * Then the notice shows nothing
 */
TEST(ContinueNotice, AStatusThatSpendsNoContinueShowsNothing) {
  ContinueNotice notice;
  notice.follow(TWO_CONTINUES, FIRST_STATUS_AT);

  notice.follow(TWO_CONTINUES, CONTINUE_SPENT_AT);

  EXPECT_EQ(notice.shownAt(CONTINUE_SPENT_AT), std::nullopt);
}

/**
 * Given 2, 1 and no continue left
 * When the line of the notice is made
 * Then each names what it counts: continues, one continue, none
 */
TEST(ContinueNotice, NamesTheContinuesLeftByTheirCount) {
  EXPECT_EQ(continueNoticeText(TWO_CONTINUES), TWO_CONTINUES_LINE);
  EXPECT_EQ(continueNoticeText(ONE_CONTINUE), ONE_CONTINUE_LINE);
  EXPECT_EQ(continueNoticeText(NO_CONTINUE), NO_CONTINUE_LINE);
}

/**
 * Given 2 continues left
 * When the notice is painted
 * Then its line is written
 */
TEST(ContinueNotice, PaintsItsLine) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  ContinueNotice::paint(canvas, TWO_CONTINUES);

  EXPECT_NE(writingOf(record, TWO_CONTINUES_LINE), nullptr);
}
