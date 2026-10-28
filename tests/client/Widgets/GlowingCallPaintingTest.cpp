#include <gtest/gtest.h>
#include "BarState.hpp"
#include "GlowingCall.hpp"
#include "GlowingCallPaintingFixtures.hpp"
#include "GlowingCallPaintingTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::BarState;
using rtype::client::GlowingCall;

/**
 * Given a call whose first letter is not typed yet
 * When it is painted
 * Then nothing is written
 */
TEST(GlowingCallPainting, WritesNothingBeforeItsFirstLetter) {
  const InterfaceCanvasRecord record =
      callPaintingOf(GlowingCall{.text = CALL_WORDS,
                                 .topLeft = CALL_PLACE,
                                 .sinceTyping = BEFORE_THE_FIRST_LETTER});

  EXPECT_TRUE(record.writings.empty());
}

/**
 * Given a call typed for two letter times
 * When it is painted
 * Then its first three letters are written, the first one typed at once
 */
TEST(GlowingCallPainting, TypesOneMoreLetterEveryLetterTime) {
  const InterfaceCanvasRecord record =
      callPaintingOf(GlowingCall{.text = CALL_WORDS,
                                 .topLeft = CALL_PLACE,
                                 .sinceTyping = TWO_LETTER_TIMES});

  ASSERT_FALSE(record.writings.empty());
  EXPECT_EQ(record.writings.back().text, FIRST_THREE_LETTERS);
}

/**
 * Given a call at rest
 * When it is painted
 * Then twelve writes in dark orange lie under one write in gold at its place
 */
TEST(GlowingCallPainting, EachLineLiesOverAGlowOfTwelveWrites) {
  const InterfaceCanvasRecord record =
      callPaintingOf(GlowingCall{.text = CALL_WORDS,
                                 .topLeft = CALL_PLACE,
                                 .sinceTyping = TWO_LETTER_TIMES});

  ASSERT_EQ(record.writings.size(), GLOW_WRITES + 1);
  EXPECT_EQ(record.writings.front().style.color, DARK_ORANGE);
  EXPECT_EQ(record.writings.back().style.color, GOLD);
  EXPECT_EQ(record.writings.back().anchor, CALL_PLACE);
}

/**
 * Given a call at rest, every letter typed, in its third blink step
 * When it is painted
 * Then nothing is written: the call blinks out
 */
TEST(GlowingCallPainting, ATypedCallIsHiddenOnItsThirdBlinkStep) {
  const InterfaceCanvasRecord record =
      callPaintingOf(GlowingCall{.text = CALL_WORDS,
                                 .topLeft = CALL_PLACE,
                                 .sinceTyping = THIRD_BLINK_STEP});

  EXPECT_TRUE(record.writings.empty());
}

/**
 * Given a steady call in its third blink step
 * When it is painted
 * Then it is written: a steady call never blinks
 */
TEST(GlowingCallPainting, ASteadyCallNeverBlinks) {
  const InterfaceCanvasRecord record =
      callPaintingOf(GlowingCall{.text = CALL_WORDS,
                                 .topLeft = CALL_PLACE,
                                 .sinceTyping = THIRD_BLINK_STEP,
                                 .steady = true});

  EXPECT_FALSE(record.writings.empty());
}

/**
 * Given a pressed call
 * When it is painted
 * Then its letters are written in star white, one pixel lower
 */
TEST(GlowingCallPainting, APressedCallIsWrittenOnePixelLower) {
  const InterfaceCanvasRecord record =
      callPaintingOf(GlowingCall{.text = CALL_WORDS,
                                 .topLeft = CALL_PLACE,
                                 .sinceTyping = TWO_LETTER_TIMES,
                                 .state = BarState::Pressed});

  ASSERT_FALSE(record.writings.empty());
  EXPECT_EQ(record.writings.back().anchor, CALL_PLACE + ONE_PIXEL_LOWER);
  EXPECT_EQ(record.writings.back().style.color, PRESSED_WHITE);
}
