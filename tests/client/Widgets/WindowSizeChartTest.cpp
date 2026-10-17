#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include "BackgroundColors.hpp"
#include "PixelSize.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "WindowSizeChart.hpp"
#include "WindowSizeSelection.hpp"

using rtype::client::paintWindowSizeChart;
using rtype::client::PixelSize;
using rtype::client::STAR_GOLD;
using rtype::client::WindowSizeSelection;

namespace {

/** @brief The chart of the options on the picture: at (36, 98), its legend
 * from (176, 106), a line every 12 pixels; on a 1920 x 1080 desktop, 1600 x
 * 900 is the window and the two largest sizes are too large. */
constexpr sf::Vector2i CHART_TOP_LEFT(36, 98);
constexpr PixelSize DESKTOP{.width = 1920, .height = 1080};
/** @brief A desktop every size fits, so every outline is solid. */
constexpr PixelSize LARGE_DESKTOP{.width = 3840, .height = 2160};
constexpr int LARGEST_SIZE_ON_THE_CHART = 128;
constexpr std::size_t SMALLEST = 0;
constexpr std::size_t WINDOW = 2;

InterfaceCanvasRecord chartOf(std::optional<std::size_t> candidate,
                              PixelSize desktop = DESKTOP) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  paintWindowSizeChart(canvas, CHART_TOP_LEFT, WindowSizeSelection(desktop),
                       candidate);
  return record;
}

}  // namespace

/**
 * Given a 1920 x 1080 desktop and nothing picked
 * When the chart is painted
 * Then its first legend line says NOW and the window size, at (176, 106)
 */
TEST(WindowSizeChartTest, TheWindowSizeComesFirstInTheLegend) {
  const InterfaceCanvasRecord record = chartOf(std::nullopt);
  const RecordedWriting* now = writingOf(record, "NOW  1600 X 900");
  ASSERT_NE(now, nullptr);
  EXPECT_EQ(now->anchor, sf::Vector2i(176, 106));
}

/**
 * Given a 1920 x 1080 desktop
 * When the chart is painted
 * Then the two sizes too large are listed under it, at 118 and 130
 */
TEST(WindowSizeChartTest, TheSizesTooLargeAreListed) {
  const InterfaceCanvasRecord record = chartOf(std::nullopt);
  const RecordedWriting* first = writingOf(record, "TOO LARGE  1920 X 1080");
  const RecordedWriting* second = writingOf(record, "TOO LARGE  2560 X 1440");
  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(first->anchor.y, 118);
  EXPECT_EQ(second->anchor.y, 130);
}

/**
 * Given 960 x 540 highlighted in the open list
 * When the chart is painted
 * Then it is listed as NEW, in gold
 */
TEST(WindowSizeChartTest, AHighlightedSizeIsListedAsNewInGold) {
  const InterfaceCanvasRecord record = chartOf(SMALLEST);
  const RecordedWriting* candidate = writingOf(record, "NEW  960 X 540");
  ASSERT_NE(candidate, nullptr);
  EXPECT_EQ(candidate->style.color, STAR_GOLD);
}

/**
 * Given the list opening, its highlight on the window size
 * When the chart is painted
 * Then nothing is listed as NEW
 */
TEST(WindowSizeChartTest, TheWindowSizeHighlightedIsNotNew) {
  const InterfaceCanvasRecord record = chartOf(WINDOW);
  EXPECT_EQ(writingOf(record, "NEW  1600 X 900"), nullptr);
}

/**
 * Given a desktop every size fits
 * When the chart is painted
 * Then the outline of the largest size starts at the top-left corner of the
 * chart and spans its 128 pixels
 */
TEST(WindowSizeChartTest, TheLargestSizeFillsTheChart) {
  const InterfaceCanvasRecord record = chartOf(std::nullopt, LARGE_DESKTOP);
  bool spansTheChart = false;
  for (const RecordedFill& fill : record.fills) {
    spansTheChart =
        spansTheChart || (fill.area.position == CHART_TOP_LEFT &&
                          fill.area.size.x == LARGEST_SIZE_ON_THE_CHART);
  }
  EXPECT_TRUE(spansTheChart);
}
