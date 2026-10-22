#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include "EffectLengths.hpp"
#include "InterfaceColors.hpp"
#include "MissionBanner.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::EDGE_BLUE;
using rtype::client::missionBannerAt;
using rtype::client::MissionBannerLook;
using rtype::client::paintMissionBanner;
using rtype::engine::Duration;

namespace {

constexpr Duration ALL_LETTERS_WRITTEN = std::chrono::milliseconds(1600);
constexpr Duration BANNER_END = std::chrono::milliseconds(2650);
constexpr std::size_t LETTERS = 13;

}  // namespace

/**
 * Given the start of a game
 * When the banner is read one nanosecond before 1 s, then at 1 s
 * Then nothing shows, then its first letter
 */
TEST(MissionBanner, TheFirstLetterShowsAtOneSecond) {
  EXPECT_EQ(missionBannerAt(BANNER_START_TIME - ONE_TICK).letters, 0U);
  EXPECT_EQ(missionBannerAt(BANNER_START_TIME).letters, 1U);
}

/**
 * Given the banner being written
 * When it is read at 1.6 s, after 13 letters of 50 ms
 * Then all thirteen letters of MISSION START show
 */
TEST(MissionBanner, EveryLetterIsWrittenAtOneLetterEveryFiftyMilliseconds) {
  EXPECT_EQ(missionBannerAt(ALL_LETTERS_WRITTEN).letters, LETTERS);
}

/**
 * Given the whole banner
 * When it is read one nanosecond before 2.65 s, then at 2.65 s
 * Then it still shows, then it is gone
 */
TEST(MissionBanner, TheBannerStaysOneSecondOnceWritten) {
  EXPECT_EQ(missionBannerAt(BANNER_END - ONE_TICK).letters, LETTERS);
  EXPECT_EQ(missionBannerAt(BANNER_END), MissionBannerLook{});
}

/**
 * Given the banner with one letter, then with every letter
 * When each is painted
 * Then the first letter stays at the same place: a new letter never moves
 * the others
 */
TEST(MissionBanner, ANewLetterNeverMovesTheOthers) {
  InterfaceCanvasRecord oneLetter;
  InterfaceCanvasRecord everyLetter;
  RecordingInterfaceCanvas first(oneLetter);
  RecordingInterfaceCanvas second(everyLetter);

  paintMissionBanner(first, missionBannerAt(BANNER_START_TIME));
  paintMissionBanner(second, missionBannerAt(ALL_LETTERS_WRITTEN));

  ASSERT_EQ(oneLetter.writings.size(), 1U);
  ASSERT_EQ(everyLetter.writings.size(), 1U);
  EXPECT_EQ(oneLetter.writings.front().text, "M");
  EXPECT_EQ(everyLetter.writings.front().text, "MISSION START");
  EXPECT_EQ(oneLetter.writings.front().anchor,
            everyLetter.writings.front().anchor);
}

/**
 * Given the whole banner
 * When it is painted
 * Then two rules of edge blue frame it, one above and one under
 */
TEST(MissionBanner, TwoRulesFrameTheBanner) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintMissionBanner(canvas, missionBannerAt(ALL_LETTERS_WRITTEN));

  ASSERT_EQ(record.fills.size(), 2U);
  EXPECT_EQ(record.fills.front().color, EDGE_BLUE);
  EXPECT_LT(record.fills.front().area.position.y,
            record.writings.front().anchor.y);
  EXPECT_GT(record.fills.back().area.position.y,
            record.writings.front().anchor.y);
}

/**
 * Given the start of a game, before the banner
 * When it is painted
 * Then nothing is painted
 */
TEST(MissionBanner, NothingIsPaintedBeforeTheBanner) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintMissionBanner(canvas, missionBannerAt(Duration::zero()));

  EXPECT_TRUE(record.fills.empty());
  EXPECT_TRUE(record.writings.empty());
}
