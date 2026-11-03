#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "RecordingInterfaceCanvas.hpp"
#include "TabStrip.hpp"
#include "TabStripFixtures.hpp"
#include "TabStripTestConstants.hpp"

using rtype::client::testing::ACCESS;
using rtype::client::testing::BETWEEN_THE_FIRST_OPTIONS_TABS;
using rtype::client::testing::CONTROLS;
using rtype::client::testing::IN_THE_SECOND_OPTIONS_TAB;
using rtype::client::testing::IN_THE_SECOND_TAB;
using rtype::client::testing::LEFT_OF_THE_SECOND_TAB;
using rtype::client::testing::ON_THE_NEXT_KEYCAP;
using rtype::client::testing::ON_THE_PREVIOUS_KEYCAP;
using rtype::client::testing::RIGHT_OF_THE_STRIP;
using rtype::client::testing::SECOND_TAB;
using rtype::client::testing::UNDER_THE_PANEL_BORDER;

/**
 * Given the four tabs of the options, SOUND open
 * When the pointer clicks inside the second tab
 * Then CONTROLS opens
 */
TEST(TabStripTest, AClickOnATabOpensIt) {
  EXPECT_EQ(optionsTabs(0).tabOpenedAt(IN_THE_SECOND_OPTIONS_TAB), CONTROLS);
}

/**
 * Given the four tabs of the options
 * When the pointer clicks in the gap between the first two
 * Then no tab opens
 */
TEST(TabStripTest, AClickBetweenTwoTabsOpensNothing) {
  EXPECT_FALSE(
      optionsTabs(0).tabOpenedAt(BETWEEN_THE_FIRST_OPTIONS_TABS).has_value());
}

/**
 * Given SOUND open
 * When the keycap on the left is clicked
 * Then ACCESS, before SOUND when wrapping around, opens
 */
TEST(TabStripTest, TheLeftKeyCapOpensThePreviousTabWrapping) {
  EXPECT_EQ(optionsTabs(0).tabOpenedAt(ON_THE_PREVIOUS_KEYCAP), ACCESS);
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

/**
 * Given a strip of two tabs, 320 pixels wide
 * When the first pixel of its second tab is clicked
 * Then that tab opens
 */
TEST(TabStripTest, TabsShareTheWidthGiven) {
  EXPECT_EQ(narrowStrip().tabOpenedAt(IN_THE_SECOND_TAB), SECOND_TAB);
}

/**
 * Given a strip of two tabs, 320 pixels wide
 * When the pixel just left of its second tab is clicked
 * Then no tab opens: it lies in the gap between the two
 */
TEST(TabStripTest, TheGapStaysBetweenTheTabs) {
  EXPECT_EQ(narrowStrip().tabOpenedAt(LEFT_OF_THE_SECOND_TAB), std::nullopt);
}

/**
 * Given a strip of two tabs, 320 pixels wide
 * When the first pixel of the keycap of the next tab is clicked
 * Then the next tab opens: the keycap moved with the width
 */
TEST(TabStripTest, TheNextKeycapEndsTheStrip) {
  EXPECT_EQ(narrowStrip().tabOpenedAt(ON_THE_NEXT_KEYCAP), SECOND_TAB);
}

/**
 * Given a strip of two tabs, 320 pixels wide
 * When the pixel just right of the strip is clicked
 * Then no tab opens
 */
TEST(TabStripTest, NothingOpensPastTheWidth) {
  EXPECT_EQ(narrowStrip().tabOpenedAt(RIGHT_OF_THE_STRIP), std::nullopt);
}
