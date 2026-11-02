#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "RecordingInterfaceCanvas.hpp"
#include "SilenceWarning.hpp"
#include "SilenceWarningTestConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::SilenceWarning;
using rtype::engine::testing::ONE_SECOND;
using testing::Contains;
using testing::Field;

/**
 * Given a server silent for just under a second
 * When the warning is read
 * Then it is not shown
 */
TEST(SilenceWarning, IsHiddenUnderTheDelay) {
  SilenceWarning warning;

  warning.advance(ONE_SECOND - ONE_NANOSECOND);

  EXPECT_FALSE(warning.isShown());
}

/**
 * Given a server silent for a second
 * When the warning is read
 * Then it is shown
 */
TEST(SilenceWarning, IsShownAtTheDelay) {
  SilenceWarning warning;

  warning.advance(ONE_SECOND);

  EXPECT_TRUE(warning.isShown());
}

/**
 * Given a warning shown
 * When a message of the server arrives
 * Then it is hidden
 */
TEST(SilenceWarning, HearingTheServerHidesIt) {
  SilenceWarning warning;
  warning.advance(ONE_SECOND);

  warning.hear();

  EXPECT_FALSE(warning.isShown());
}

/**
 * Given a stopped warning
 * When a second of silence passes
 * Then it is not shown
 */
TEST(SilenceWarning, AStoppedWarningNeverShows) {
  SilenceWarning warning;
  warning.stop();

  warning.advance(ONE_SECOND);

  EXPECT_FALSE(warning.isShown());
}

/**
 * Given a warning shown
 * When it is painted
 * Then WAITING FOR THE SERVER is written beside an image of the HUD sheet
 */
TEST(SilenceWarning, PaintsTheWordsAndTheBars) {
  SilenceWarning warning;
  warning.advance(ONE_SECOND);
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  warning.paint(canvas);

  EXPECT_NE(writingOf(record, WARNING_WORDS), nullptr);
  EXPECT_THAT(record.images,
              Contains(Field(&RecordedImage::assetId, HUD_SHEET)));
}

/**
 * Given a warning hidden
 * When it is painted
 * Then nothing is written
 */
TEST(SilenceWarning, PaintsNothingWhileHidden) {
  const SilenceWarning warning;
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  warning.paint(canvas);

  EXPECT_TRUE(record.writings.empty());
}
