#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include "AudioConstants.hpp"
#include "AudioTestConstants.hpp"
#include "FakeMusicClock.hpp"
#include "ScreenTestConstants.hpp"
#include "SyncedClock.hpp"
#include "TimeConstants.hpp"

using rtype::client::MUSIC_SYNC_SNAP;
using rtype::client::SyncedClock;
using rtype::client::testing::A_FRAME;
using rtype::client::testing::DURATION_ROUNDING;
using rtype::client::testing::MUSIC_AT_SKIP;
using rtype::client::testing::MUSIC_BEHIND;
using rtype::client::testing::MUSIC_FAR_AHEAD;
using rtype::client::testing::SKIP_TARGET;
using rtype::client::testing::SMALL_GAP;
using rtype::client::testing::TENTH_OF_THE_SMALL_GAP;
using rtype::client::testing::TENTH_OF_THE_SNAP;

/**
 * Given no music playing
 * When three frames pass
 * Then the clock moved by the three frames
 */
TEST(SyncedClock, WithoutMusicTheClockFollowsTheFrames) {
  const FakeMusicClock music;
  SyncedClock clock(music);

  clock.advance(A_FRAME);
  clock.advance(A_FRAME);
  clock.advance(A_FRAME);

  EXPECT_EQ(clock.now(), A_FRAME + A_FRAME + A_FRAME);
}

/**
 * Given music 200 ms in
 * When one frame passes
 * Then the clock takes the position of the music at once
 */
TEST(SyncedClock, TheClockTakesTheMusicWhenItIsFarOff) {
  FakeMusicClock music;
  music.setPlayingOffset(MUSIC_FAR_AHEAD);
  SyncedClock clock(music);

  clock.advance(A_FRAME);

  EXPECT_EQ(clock.now(), MUSIC_FAR_AHEAD);
}

/**
 * Given music 40 ms ahead of the clock after one frame of 16 ms
 * When the frame passes
 * Then the clock closes a tenth of the gap, 16 / 160
 */
TEST(SyncedClock, AFrameClosesPartOfASmallGap) {
  FakeMusicClock music;
  music.setPlayingOffset(A_FRAME + SMALL_GAP);
  SyncedClock clock(music);

  clock.advance(A_FRAME);

  EXPECT_LE(std::chrono::abs(clock.now() - (A_FRAME + TENTH_OF_THE_SMALL_GAP)),
            DURATION_ROUNDING);
}

/**
 * Given music exactly MUSIC_SYNC_SNAP ahead of the clock after one frame
 * When the frame passes
 * Then the clock closes a tenth of the gap, not all of it
 */
TEST(SyncedClock, AGapOfExactlyTheSnapIsClosedInPart) {
  FakeMusicClock music;
  music.setPlayingOffset(A_FRAME + MUSIC_SYNC_SNAP);
  SyncedClock clock(music);

  clock.advance(A_FRAME);

  EXPECT_LE(std::chrono::abs(clock.now() - (A_FRAME + TENTH_OF_THE_SNAP)),
            DURATION_ROUNDING);
}

/**
 * Given music one nanosecond further than MUSIC_SYNC_SNAP ahead of the clock
 * after one frame
 * When the frame passes
 * Then the clock takes the position of the music at once
 */
TEST(SyncedClock, AGapJustOverTheSnapTakesTheMusic) {
  FakeMusicClock music;
  music.setPlayingOffset(A_FRAME + MUSIC_SYNC_SNAP + ONE_TICK);
  SyncedClock clock(music);

  clock.advance(A_FRAME);

  EXPECT_EQ(clock.now(), A_FRAME + MUSIC_SYNC_SNAP + ONE_TICK);
}

/**
 * Given a clock on the music, then music 90 ms behind it after one more frame
 * When that frame passes
 * Then the clock still moves forward: closing part of a gap never moves it
 * back
 */
TEST(SyncedClock, ClosingPartOfAGapNeverMovesTheTimeBack) {
  FakeMusicClock music;
  music.setPlayingOffset(MUSIC_FAR_AHEAD);
  SyncedClock clock(music);
  clock.advance(A_FRAME);

  music.setPlayingOffset(MUSIC_FAR_AHEAD + A_FRAME - MUSIC_BEHIND);
  clock.advance(A_FRAME);

  EXPECT_GT(clock.now(), MUSIC_FAR_AHEAD);
}

/**
 * Given music 40 ms ahead of the clock after one frame
 * When the frame passes
 * Then the gap left to the music is the nine tenths not closed yet
 */
TEST(SyncedClock, TheGapLeftIsWhatTheFrameDidNotClose) {
  FakeMusicClock music;
  music.setPlayingOffset(A_FRAME + SMALL_GAP);
  SyncedClock clock(music);

  clock.advance(A_FRAME);

  const std::optional<rtype::engine::Duration> gap = clock.gapToMusic();
  ASSERT_TRUE(gap.has_value());
  EXPECT_LE(std::chrono::abs(gap.value_or(rtype::engine::Duration{}) -
                             (SMALL_GAP - TENTH_OF_THE_SMALL_GAP)),
            DURATION_ROUNDING);
}

/**
 * Given no music playing
 * When the gap to the music is read
 * Then there is none
 */
TEST(SyncedClock, WithoutMusicThereIsNoGap) {
  const FakeMusicClock music;
  const SyncedClock clock(music);

  EXPECT_FALSE(clock.gapToMusic().has_value());
}

/**
 * Given music 1 s in, and the clock sent to the slam of the logo
 * When the music and the clock move on by one frame
 * Then the clock stays ahead of the music by the jump
 */
TEST(SyncedClock, AJumpLeavesTheMusicWhereItIs) {
  FakeMusicClock music;
  music.setPlayingOffset(MUSIC_AT_SKIP);
  SyncedClock clock(music);
  clock.jumpTo(SKIP_TARGET);

  music.setPlayingOffset(MUSIC_AT_SKIP + A_FRAME);
  clock.advance(A_FRAME);

  EXPECT_EQ(clock.now(), SKIP_TARGET + A_FRAME);
}
