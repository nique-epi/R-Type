#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "RecordingInterfaceCanvas.hpp"
#include "TabStrip.hpp"

using rtype::client::TabStrip;

namespace {

/** @brief The tabs of the options on the picture the team chose: four of 96
 * pixels at 42, 142, 242 and 342, the strip at (24, 50). */
constexpr sf::Vector2i STRIP_TOP_LEFT(24, 50);
constexpr int TAB_TOP_ROW = 55;
constexpr std::size_t CONTROLS = 1;
constexpr std::size_t ACCESS = 3;
/** @brief The first row under the border of the panel, at row 66. */
constexpr int UNDER_THE_PANEL_BORDER = 67;

TabStrip optionsTabs(std::size_t open) {
  return {{"SOUND", "CONTROLS", "DISPLAY", "ACCESS"},
          open,
          {.previous = "Q", .next = "E"},
          STRIP_TOP_LEFT};
}

}  // namespace

/**
 * Given the four tabs of the options, SOUND open
 * When the pointer clicks inside the second tab
 * Then CONTROLS opens
 */
TEST(TabStripTest, AClickOnATabOpensIt) {
  EXPECT_EQ(optionsTabs(0).tabOpenedAt({150, TAB_TOP_ROW}), CONTROLS);
}

/**
 * Given the four tabs of the options
 * When the pointer clicks in the gap between the first two
 * Then no tab opens
 */
TEST(TabStripTest, AClickBetweenTwoTabsOpensNothing) {
  EXPECT_FALSE(optionsTabs(0).tabOpenedAt({140, TAB_TOP_ROW}).has_value());
}

/**
 * Given SOUND open
 * When the keycap on the left is clicked
 * Then ACCESS, before SOUND when wrapping around, opens
 */
TEST(TabStripTest, TheLeftKeyCapOpensThePreviousTabWrapping) {
  EXPECT_EQ(optionsTabs(0).tabOpenedAt({30, TAB_TOP_ROW}), ACCESS);
}

/**
 * Given ACCESS open
 * When the next tab is asked for
 * Then SOUND, after ACCESS when wrapping around, is it
 */
TEST(TabStripTest, TheNextTabWrapsAround) {
  EXPECT_EQ(optionsTabs(ACCESS).nextTab(), 0U);
}

/**
 * Given CONTROLS open
 * When the strip is painted
 * Then the open tab is painted last, over its neighbours
 */
TEST(TabStripTest, TheOpenTabIsPaintedLast) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  optionsTabs(CONTROLS).paint(canvas);
  ASSERT_FALSE(record.writings.empty());
  EXPECT_EQ(record.writings.back().text, "CONTROLS");
}

/**
 * Given SOUND open
 * When the strip is painted
 * Then the open tab reaches row 66, the border of the panel under it
 */
TEST(TabStripTest, TheOpenTabCoversTheBorderOfThePanel) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  optionsTabs(0).paint(canvas);
  bool reachesThePanel = false;
  for (const RecordedFill& fill : record.fills) {
    reachesThePanel =
        reachesThePanel ||
        fill.area.position.y + fill.area.size.y == UNDER_THE_PANEL_BORDER;
  }
  EXPECT_TRUE(reachesThePanel);
}
