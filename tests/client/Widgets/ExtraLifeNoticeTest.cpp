#include <gtest/gtest.h>
#include <optional>
#include "ExtraLifeNotice.hpp"
#include "ExtraLifeNoticeTestConstants.hpp"
#include "ExtraLifeRises.hpp"
#include "GameEvent.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::ExtraLifeNotice;
using rtype::client::ExtraLifeRises;
using rtype::network::GameEvent;
using rtype::network::GameEventKind;

/**
 * Given the second player who won an extra life
 * When the notices are read at once
 * Then the bay of that player shows one, not risen yet, and no other
 */
TEST(ExtraLifeNotice, ShowsOverTheBayOfThePlayerWhoWonIt) {
  ExtraLifeNotice notice;

  notice.apply(GameEvent{.kind = GameEventKind::ExtraLifeWon,
                         .playerNumber = SECOND_PLAYER},
               LIFE_WON_AT);

  ExtraLifeRises expected{};
  expected.at(SECOND_BAY) = NOT_RISEN;
  EXPECT_EQ(notice.risesAt(LIFE_WON_AT), expected);
}

/**
 * Given an extra life won
 * When half the time of its notice passes
 * Then the notice has risen half its 6 rows
 */
TEST(ExtraLifeNotice, RisesOverItsTime) {
  ExtraLifeNotice notice;
  notice.apply(GameEvent{.kind = GameEventKind::ExtraLifeWon,
                         .playerNumber = SECOND_PLAYER},
               LIFE_WON_AT);

  EXPECT_EQ(notice.risesAt(LIFE_WON_AT + HALF_THE_NOTICE).at(SECOND_BAY),
            std::optional<int>{HALF_RISEN});
}

/**
 * Given an extra life won
 * When 1.5 seconds pass
 * Then its notice is gone
 */
TEST(ExtraLifeNotice, IsGoneAfterItsTime) {
  ExtraLifeNotice notice;
  notice.apply(GameEvent{.kind = GameEventKind::ExtraLifeWon,
                         .playerNumber = SECOND_PLAYER},
               LIFE_WON_AT);

  EXPECT_EQ(notice.risesAt(LIFE_WON_AT + EXTRA_LIFE_SHOWN), ExtraLifeRises{});
}

/**
 * Given the second player hit
 * When the notices are read
 * Then none shows: only an extra life starts one
 */
TEST(ExtraLifeNotice, AnotherEventShowsNothing) {
  ExtraLifeNotice notice;

  notice.apply(GameEvent{.kind = GameEventKind::PlayerHit,
                         .playerNumber = SECOND_PLAYER},
               LIFE_WON_AT);

  EXPECT_EQ(notice.risesAt(LIFE_WON_AT), ExtraLifeRises{});
}

/**
 * Given the notice of the second player
 * When it is painted
 * Then 1UP is written at the column of the lives of the second bay
 */
TEST(ExtraLifeNotice, PaintsOneUpOverTheLivesOfItsBay) {
  ExtraLifeRises rises{};
  rises.at(SECOND_BAY) = NOT_RISEN;
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  ExtraLifeNotice::paint(canvas, rises);

  const RecordedWriting* const written = writingOf(record, EXTRA_LIFE_WORD);
  ASSERT_NE(written, nullptr);
  EXPECT_EQ(written->anchor.x, SECOND_BAY_LIVES_COLUMN);
}
