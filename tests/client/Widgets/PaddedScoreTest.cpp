#include <gtest/gtest.h>
#include "PaddedScore.hpp"
#include "PaddedScoreTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::paintPaddedScore;
using rtype::client::testing::DIGIT_COLOR;
using rtype::client::testing::DOUBLE_SCALE;
using rtype::client::testing::EIGHT_DIGIT_SCORE;
using rtype::client::testing::FIVE_DIGIT_SCORE;
using rtype::client::testing::LARGEST_SHOWN;
using rtype::client::testing::SCALED_ZEROS_END;
using rtype::client::testing::SCORE_END;
using rtype::client::testing::TWO_PADDING_ZEROS;
using rtype::client::testing::ZERO_COLOR;
using rtype::client::testing::ZEROS_END;

/**
 * Given a score of five digits
 * When it is painted
 * Then two zeros in the zero color end where its digits start
 */
TEST(PaddedScore, ZerosPadAScoreToSevenDigits) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintPaddedScore(canvas, {.value = FIVE_DIGIT_SCORE,
                            .end = SCORE_END,
                            .digitColor = DIGIT_COLOR,
                            .zeroColor = ZERO_COLOR});

  const RecordedWriting* zeros = writingOf(record, TWO_PADDING_ZEROS);
  ASSERT_NE(zeros, nullptr);
  EXPECT_EQ(zeros->anchor, ZEROS_END);
  EXPECT_EQ(zeros->style.color, ZERO_COLOR);
}

/**
 * Given a score of five digits written at scale 2
 * When it is painted
 * Then its zeros end twice as far from its end
 */
TEST(PaddedScore, AScaledScoreMovesItsZerosByItsScaledWidth) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintPaddedScore(canvas, {.value = FIVE_DIGIT_SCORE,
                            .end = SCORE_END,
                            .digitColor = DIGIT_COLOR,
                            .zeroColor = ZERO_COLOR,
                            .scale = DOUBLE_SCALE});

  const RecordedWriting* zeros = writingOf(record, TWO_PADDING_ZEROS);
  ASSERT_NE(zeros, nullptr);
  EXPECT_EQ(zeros->anchor, SCALED_ZEROS_END);
}

/**
 * Given a score above 9999999
 * When it is painted
 * Then 9999999 is written
 */
TEST(PaddedScore, AHigherScoreStopsAtTheLargestShown) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintPaddedScore(canvas, {.value = EIGHT_DIGIT_SCORE,
                            .end = SCORE_END,
                            .digitColor = DIGIT_COLOR,
                            .zeroColor = ZERO_COLOR});

  EXPECT_NE(writingOf(record, LARGEST_SHOWN), nullptr);
}
