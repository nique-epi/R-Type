#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <string>
#include "RecordingInterfaceCanvas.hpp"
#include "SpeechKeyName.hpp"
#include "SpeechLines.hpp"
#include "SpeechLinesConstants.hpp"
#include "SpeechLinesTestConstants.hpp"
#include "SpeechWordLook.hpp"
#include "WidgetText.hpp"

using rtype::client::paintSpeech;
using rtype::client::regularTextWidth;
using rtype::client::SPEECH_LINE_HEIGHT;
using rtype::client::SPEECH_WORD_GAP;
using rtype::client::SpeechKeyName;
using rtype::client::speechLetterCountOf;
using rtype::client::SpeechLines;
using rtype::client::speechLinesOf;
using rtype::client::SpeechWordLook;
using rtype::client::testing::GREETING;
using rtype::client::testing::GREETING_FIRST_WORD;
using rtype::client::testing::GREETING_FIRST_WORD_LETTERS;
using rtype::client::testing::GREETING_SECOND_WORD;
using rtype::client::testing::PRESS_KEY_SENTENCE;
using rtype::client::testing::PRESS_KEY_SENTENCE_LETTERS;
using rtype::client::testing::RADIO_KEY;
using rtype::client::testing::RADIO_SENTENCE;
using rtype::client::testing::RADIO_WORD;
using rtype::client::testing::SHOOT_KEY;
using rtype::client::testing::SHOOT_SENTENCE;
using rtype::client::testing::SHOOT_SENTENCE_LETTERS;
using rtype::client::testing::SHOOT_TOKEN;
using rtype::client::testing::SPEECH_TOP_LEFT;
using rtype::client::testing::WIDE_BUBBLE_TEXT;

/**
 * Given a sentence ending in {SHOOT}, and SHOOT named SPACE
 * When it is cut into lines
 * Then its keycap shows SPACE, the dot written after it
 */
TEST(SpeechLinesTest, AKeyCapShowsTheKeyItsTokenNames) {
  const std::array keyNames{
      SpeechKeyName{.token = SHOOT_TOKEN, .name = std::string(SHOOT_KEY)}};
  const SpeechLines lines =
      speechLinesOf(SHOOT_SENTENCE, keyNames, WIDE_BUBBLE_TEXT);
  ASSERT_EQ(lines.size(), 1U);
  ASSERT_EQ(lines.front().size(), 2U);
  EXPECT_EQ(lines.front().back().text, SHOOT_KEY);
  EXPECT_EQ(lines.front().back().look, SpeechWordLook::KeyCap);
  EXPECT_EQ(lines.front().back().suffix, ".");
}

/**
 * Given a sentence with a starred word and a keycap no key name gives
 * When it is cut into lines
 * Then the word is highlighted without its stars, and the keycap shows its
 * own token
 */
TEST(SpeechLinesTest, AStarredWordIsHighlightedAndAnUnnamedKeyShowsItself) {
  const SpeechLines lines = speechLinesOf(RADIO_SENTENCE, {}, WIDE_BUBBLE_TEXT);
  ASSERT_EQ(lines.size(), 1U);
  ASSERT_EQ(lines.front().size(), 2U);
  EXPECT_EQ(lines.front().front().text, RADIO_WORD);
  EXPECT_EQ(lines.front().front().look, SpeechWordLook::Highlighted);
  EXPECT_EQ(lines.front().front().suffix, "!");
  EXPECT_EQ(lines.front().back().text, RADIO_KEY);
}

/**
 * Given two words and a width just wide enough for both, then a pixel less
 * When the sentence is cut into lines
 * Then it takes one line, then two
 */
TEST(SpeechLinesTest, AWordThatPassesTheWidthGoesToTheNextLine) {
  const int bothWords = regularTextWidth(GREETING_FIRST_WORD) +
                        SPEECH_WORD_GAP +
                        regularTextWidth(GREETING_SECOND_WORD);
  EXPECT_EQ(speechLinesOf(GREETING, {}, bothWords).size(), 1U);
  EXPECT_EQ(speechLinesOf(GREETING, {}, bothWords - 1).size(), 2U);
}

/**
 * Given a sentence of five letters, a keycap and a dot
 * When its letters are counted
 * Then the keycap counts as one
 */
TEST(SpeechLinesTest, AKeyCapCountsAsOneLetter) {
  const std::array keyNames{
      SpeechKeyName{.token = SHOOT_TOKEN, .name = std::string(SHOOT_KEY)}};
  EXPECT_EQ(speechLetterCountOf(
                speechLinesOf(SHOOT_SENTENCE, keyNames, WIDE_BUBBLE_TEXT)),
            SHOOT_SENTENCE_LETTERS);
}

/**
 * Given a greeting
 * When none of its letters show
 * Then nothing is written
 */
TEST(SpeechLinesTest, NoLetterWritesNothing) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  paintSpeech(canvas, speechLinesOf(GREETING, {}, WIDE_BUBBLE_TEXT),
              {.topLeft = SPEECH_TOP_LEFT, .letters = 0});
  EXPECT_TRUE(record.writings.empty());
}

/**
 * Given a greeting
 * When its first three letters show
 * Then only they are written, where the sentence starts
 */
TEST(SpeechLinesTest, OnlyTheLettersShownAreWritten) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  paintSpeech(canvas, speechLinesOf(GREETING, {}, WIDE_BUBBLE_TEXT),
              {.topLeft = SPEECH_TOP_LEFT, .letters = 3});
  ASSERT_EQ(record.writings.size(), 1U);
  EXPECT_EQ(record.writings.front().text, "PRI");
  EXPECT_EQ(record.writings.front().anchor, SPEECH_TOP_LEFT);
}

/**
 * Given a greeting
 * When its first word and one more letter show
 * Then the second word starts a gap after the first
 */
TEST(SpeechLinesTest, TheSecondWordStartsAGapAfterTheFirst) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  paintSpeech(
      canvas, speechLinesOf(GREETING, {}, WIDE_BUBBLE_TEXT),
      {.topLeft = SPEECH_TOP_LEFT, .letters = GREETING_FIRST_WORD_LETTERS + 1});
  ASSERT_EQ(record.writings.size(), 2U);
  EXPECT_EQ(record.writings.front().text, GREETING_FIRST_WORD);
  EXPECT_EQ(record.writings.back().text, "P");
  EXPECT_EQ(record.writings.back().anchor.x,
            SPEECH_TOP_LEFT.x + regularTextWidth(GREETING_FIRST_WORD) +
                SPEECH_WORD_GAP);
}

/**
 * Given a greeting cut on two lines
 * When all of it shows
 * Then the second line is written a line lower
 */
TEST(SpeechLinesTest, TheNextLineIsALineLower) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  const SpeechLines lines =
      speechLinesOf(GREETING, {}, regularTextWidth(GREETING_FIRST_WORD));
  paintSpeech(
      canvas, lines,
      {.topLeft = SPEECH_TOP_LEFT, .letters = speechLetterCountOf(lines)});
  ASSERT_EQ(record.writings.size(), 2U);
  EXPECT_EQ(record.writings.back().anchor,
            SPEECH_TOP_LEFT + sf::Vector2i(0, SPEECH_LINE_HEIGHT));
}

/**
 * Given a sentence ending in a keycap
 * When every letter but the keycap shows, then the keycap too
 * Then the keycap is written only the second time
 */
TEST(SpeechLinesTest, AKeyCapShowsOnceReached) {
  const SpeechLines lines =
      speechLinesOf(PRESS_KEY_SENTENCE, {}, WIDE_BUBBLE_TEXT);
  InterfaceCanvasRecord before;
  RecordingInterfaceCanvas beforeCanvas(before);
  paintSpeech(
      beforeCanvas, lines,
      {.topLeft = SPEECH_TOP_LEFT, .letters = PRESS_KEY_SENTENCE_LETTERS - 1});
  InterfaceCanvasRecord after;
  RecordingInterfaceCanvas afterCanvas(after);
  paintSpeech(
      afterCanvas, lines,
      {.topLeft = SPEECH_TOP_LEFT, .letters = PRESS_KEY_SENTENCE_LETTERS});
  ASSERT_EQ(before.writings.size(), 1U);
  ASSERT_EQ(after.writings.size(), 2U);
  EXPECT_EQ(after.writings.back().text, RADIO_KEY);
}
