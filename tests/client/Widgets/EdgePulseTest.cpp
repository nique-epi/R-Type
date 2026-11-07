#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <optional>
#include "EdgePulse.hpp"
#include "EdgePulseTestConstants.hpp"
#include "FlashPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::edgePulseAt;
using rtype::client::EdgePulseLook;
using rtype::client::FlashPreference;
using rtype::client::paintEdgePulse;

/**
 * Given the pulse running
 * When the edges are read 400 ms into it
 * Then they are tinted at 70 %
 */
TEST(EdgePulse, TheEdgesPeakHalfAPeriodIn) {
  EXPECT_EQ(
      edgePulseAt(PULSE_PEAK_TIME, std::nullopt, FlashPreference::Full).alpha,
      PULSE_PEAK_ALPHA);
}

/**
 * Given the WARNING screen starting
 * When the edges are read at its start
 * Then they are clear
 */
TEST(EdgePulse, TheEdgesAreClearWhenTheWarningStarts) {
  EXPECT_EQ(edgePulseAt(PULSE_START, std::nullopt, FlashPreference::Full).alpha,
            0);
}

/**
 * Given the pulse running
 * When the edges are read a whole period of 800 ms into it
 * Then they are clear again
 */
TEST(EdgePulse, TheEdgesAreClearAgainAfterAWholePeriod) {
  EXPECT_EQ(
      edgePulseAt(PULSE_WHOLE_PERIOD_TIME, std::nullopt, FlashPreference::Full)
          .alpha,
      0);
}

/**
 * Given the pulse running, flashes reduced
 * When the edges are read at its peak
 * Then they are clear
 */
TEST(EdgePulse, WithFlashesReducedTheEdgesNeverPulse) {
  EXPECT_EQ(edgePulseAt(PULSE_PEAK_TIME, std::nullopt, FlashPreference::Reduced)
                .alpha,
            0);
}

/**
 * Given the boss roaring, long after the WARNING screen
 * When the edges are read as it roars
 * Then they are tinted at 40 %
 */
TEST(EdgePulse, TheRoarFlashesTheEdges) {
  EXPECT_EQ(
      edgePulseAt(LONG_AFTER_THE_WARNING, AT_THE_ROAR, FlashPreference::Full)
          .alpha,
      ROAR_FLASH_ALPHA);
}

/**
 * Given the boss that roared
 * When the edges are read 200 ms, then 400 ms after the roar
 * Then they are tinted at half of 40 %, then clear
 */
TEST(EdgePulse, TheRoarFlashFadesInFourHundredMilliseconds) {
  EXPECT_EQ(edgePulseAt(LONG_AFTER_THE_WARNING, HALFWAY_THROUGH_THE_ROAR_FLASH,
                        FlashPreference::Full)
                .alpha,
            HALF_THE_ROAR_FLASH_ALPHA);
  EXPECT_EQ(edgePulseAt(LONG_AFTER_THE_WARNING, ROAR_FLASH_OVER,
                        FlashPreference::Full)
                .alpha,
            0);
}

/**
 * Given the boss roaring, flashes reduced
 * When the edges are read as it roars
 * Then they are clear
 */
TEST(EdgePulse, WithFlashesReducedTheRoarDoesNotFlash) {
  EXPECT_EQ(
      edgePulseAt(LONG_AFTER_THE_WARNING, AT_THE_ROAR, FlashPreference::Reduced)
          .alpha,
      0);
}

/**
 * Given the edges at the peak of the pulse
 * When they are painted
 * Then the strip on the left edge has the whole tint, the next one less
 */
TEST(EdgePulse, TheTintIsStrongestOnTheEdge) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintEdgePulse(canvas, EdgePulseLook{.alpha = PULSE_PEAK_ALPHA});

  ASSERT_GE(record.fills.size(), 2U);
  const auto second =
      std::ranges::find_if(record.fills, [](const RecordedFill& fill) {
        return fill.area.position.x == SECOND_STRIP_COLUMN;
      });
  ASSERT_NE(second, record.fills.end());
  EXPECT_EQ(record.fills.front().area.position.x, 0);
  EXPECT_EQ(record.fills.front().color.a, PULSE_PEAK_ALPHA);
  EXPECT_LT(second->color.a, PULSE_PEAK_ALPHA);
}

/**
 * Given the edges at the peak of the pulse
 * When they are painted
 * Then no strip reaches the status band
 */
TEST(EdgePulse, NoStripReachesTheStatusBand) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintEdgePulse(canvas, EdgePulseLook{.alpha = PULSE_PEAK_ALPHA});

  for (const RecordedFill& fill : record.fills) {
    EXPECT_LE(fill.area.position.y + fill.area.size.y,
              PLAYFIELD_ROWS_OVER_THE_BAND);
  }
}
