#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "DepartureFeed.hpp"
#include "DepartureFeedFixtures.hpp"
#include "DepartureFeedTestConstants.hpp"
#include "PilotStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "StatusBandTestConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::DepartureFeed;
using rtype::client::PilotState;
using rtype::client::testing::PLUG_ICON;
using testing::AllOf;
using testing::Contains;
using testing::Field;

/**
 * Given a feed and a pilot who left
 * When the feed follows the pilots
 * Then it holds one line, for that pilot
 */
TEST(DepartureFeed, APilotWhoLeftGetsALine) {
  DepartureFeed feed;

  feed.follow(std::vector{pilotIn(NOVA_NUMBER, NOVA, PilotState::Left)});

  ASSERT_EQ(feed.lines().size(), 1U);
  EXPECT_EQ(feed.lines().front().name, NOVA);
}

/**
 * Given a feed that announced a pilot who left
 * When it follows the same pilots again
 * Then it still holds one line: one per pilot and per game
 */
TEST(DepartureFeed, APilotIsAnnouncedOnce) {
  DepartureFeed feed;
  const std::vector pilots{pilotIn(NOVA_NUMBER, NOVA, PilotState::Left)};
  feed.follow(pilots);

  feed.follow(pilots);

  EXPECT_EQ(feed.lines().size(), 1U);
}

/**
 * Given a feed
 * When it follows a pilot still flying
 * Then it holds no line
 */
TEST(DepartureFeed, APilotStillFlyingGetsNoLine) {
  DepartureFeed feed;

  feed.follow(std::vector{pilotIn(NOVA_NUMBER, NOVA, PilotState::Flying)});

  EXPECT_TRUE(feed.lines().empty());
}

/**
 * Given a feed that announced NOVA
 * When KAI loses its signal
 * Then the line of KAI is on top
 */
TEST(DepartureFeed, TheNewestLineIsOnTop) {
  DepartureFeed feed;
  feed.follow(std::vector{pilotIn(NOVA_NUMBER, NOVA, PilotState::Left)});

  feed.follow(std::vector{pilotIn(NOVA_NUMBER, NOVA, PilotState::Left),
                          pilotIn(KAI_NUMBER, KAI, PilotState::SignalLost)});

  ASSERT_EQ(feed.lines().size(), 2U);
  EXPECT_EQ(feed.lines().front().playerNumber, KAI_NUMBER);
}

/**
 * Given a feed holding the most lines it shows
 * When one more pilot leaves
 * Then it still holds the most lines, the oldest one gone
 */
TEST(DepartureFeed, OneDepartureTooManyDropsTheOldestLine) {
  DepartureFeed feed;
  for (std::uint8_t number = 1; number <= MOST_LINES + 1; ++number) {
    feed.follow(std::vector{pilotIn(number, NOVA, PilotState::Left)});
  }

  ASSERT_EQ(feed.lines().size(), MOST_LINES);
  EXPECT_EQ(feed.lines().back().playerNumber, OLDEST_KEPT_NUMBER);
}

/**
 * Given a line of the feed
 * When one nanosecond less than its time passes
 * Then it is still shown
 */
TEST(DepartureFeed, ALineStaysUntilItsTime) {
  DepartureFeed feed;
  feed.follow(std::vector{pilotIn(NOVA_NUMBER, NOVA, PilotState::Left)});

  feed.advance(LINE_TIME - ONE_NANOSECOND);

  EXPECT_EQ(feed.lines().size(), 1U);
}

/**
 * Given a line of the feed
 * When its time passes
 * Then it is gone
 */
TEST(DepartureFeed, ALineGoesAtItsTime) {
  DepartureFeed feed;
  feed.follow(std::vector{pilotIn(NOVA_NUMBER, NOVA, PilotState::Left)});

  feed.advance(LINE_TIME);

  EXPECT_TRUE(feed.lines().empty());
}

/**
 * Given a feed painted once
 * When a pilot leaves
 * Then its revision changed, so the screen repaints it
 */
TEST(DepartureFeed, ANewLineChangesTheRevision) {
  DepartureFeed feed;
  const auto painted = feed.revision();

  feed.follow(std::vector{pilotIn(NOVA_NUMBER, NOVA, PilotState::Left)});

  EXPECT_NE(feed.revision(), painted);
}

/**
 * Given a feed with the line of NOVA, who left
 * When it is painted
 * Then the name of NOVA and LEFT THE GAME are written
 */
TEST(DepartureFeed, PaintsTheNameAndTheWords) {
  const InterfaceCanvasRecord record =
      feedPaintingOf(pilotIn(NOVA_NUMBER, NOVA, PilotState::Left));

  EXPECT_NE(writingOf(record, NOVA), nullptr);
  EXPECT_NE(writingOf(record, FEED_LEFT_WORDS), nullptr);
}

/**
 * Given a feed with the line of KAI, whose signal was lost
 * When it is painted
 * Then the plug of the status band is drawn
 */
TEST(DepartureFeed, ALostSignalDrawsThePlugOfTheBand) {
  const InterfaceCanvasRecord record =
      feedPaintingOf(pilotIn(KAI_NUMBER, KAI, PilotState::SignalLost));

  EXPECT_THAT(record.smoothImages,
              Contains(Field(&RecordedSmoothImage::assetId, PLUG_ICON)));
}

/**
 * Given a feed with the line of a pilot whose nickname has sixteen letters
 * When it is painted
 * Then its first ten letters are written
 */
TEST(DepartureFeed, ALongNameIsCutToTenLetters) {
  const InterfaceCanvasRecord record =
      feedPaintingOf(pilotIn(NOVA_NUMBER, LONG_NAME, PilotState::Left));

  EXPECT_NE(writingOf(record, LONG_NAME_CUT), nullptr);
}

/**
 * Given a feed with the line of KAI, whose signal was lost
 * When it is painted
 * Then the plug starts on the row of the name and ends at the right of the
 * icon column, as the design aligns them
 */
TEST(DepartureFeed, ThePlugSitsOnTheRowOfTheName) {
  const InterfaceCanvasRecord record =
      feedPaintingOf(pilotIn(KAI_NUMBER, KAI, PilotState::SignalLost));

  const RecordedWriting* name = writingOf(record, KAI);
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->anchor, FIRST_NAME_ANCHOR);
  EXPECT_THAT(
      record.smoothImages,
      Contains(AllOf(Field(&RecordedSmoothImage::assetId, PLUG_ICON),
                     Field(&RecordedSmoothImage::area, FIRST_PLUG_AREA))));
}

/**
 * Given a feed with the line of KAI, whose signal was lost
 * When it is painted
 * Then the words are written on the row of the name, so both end on the
 * same baseline
 */
TEST(DepartureFeed, TheWordsShareTheRowOfTheName) {
  const InterfaceCanvasRecord record =
      feedPaintingOf(pilotIn(KAI_NUMBER, KAI, PilotState::SignalLost));

  const RecordedWriting* words = writingOf(record, FEED_LOST_WORDS);
  ASSERT_NE(words, nullptr);
  EXPECT_EQ(words->anchor.y, FIRST_NAME_ANCHOR.y);
}
