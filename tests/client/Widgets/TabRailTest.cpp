#include <gtest/gtest.h>
#include <optional>
#include "RecordingInterfaceCanvas.hpp"
#include "TabRail.hpp"
#include "TabRailFixtures.hpp"
#include "TabRailTestConstants.hpp"

using rtype::client::TabRail;
using rtype::client::testing::BETWEEN_THE_FIRST_TWO_RAIL_TABS;
using rtype::client::testing::FIRST_RAIL_LABEL;
using rtype::client::testing::FIRST_RAIL_TAB_LEFT;
using rtype::client::testing::FIRST_RAIL_TAB_RIGHT;
using rtype::client::testing::IDLE_LABEL_COLOR;
using rtype::client::testing::IN_THE_SECOND_RAIL_TAB;
using rtype::client::testing::INDICATOR_COLOR;
using rtype::client::testing::INSIDE_THE_SHAKE;
using rtype::client::testing::LOCKED_BADGE;
using rtype::client::testing::LOCKED_LABEL_COLOR;
using rtype::client::testing::LOCKED_RAIL_LABEL;
using rtype::client::testing::LOCKED_RAIL_TAB;
using rtype::client::testing::OPEN_LABEL_COLOR;
using rtype::client::testing::PAST_THE_SHAKE;
using rtype::client::testing::RAIL_TOP_MIDDLE;
using rtype::client::testing::railPaintingOf;
using rtype::client::testing::SECOND_RAIL_LABEL;
using rtype::client::testing::SECOND_RAIL_TAB;
using rtype::client::testing::threeTabRail;
using rtype::client::testing::UNDER_THE_RAIL;

/**
 * Given the rail of the tests, ONE open
 * When it is painted
 * Then ONE is written in star white at the left of the row, at its top
 */
TEST(TabRailTest, TheOpenTabIsWrittenInStarWhite) {
  const InterfaceCanvasRecord painting = railPaintingOf(threeTabRail());
  const RecordedWriting* label = writingOf(painting, FIRST_RAIL_LABEL);

  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->style.color, OPEN_LABEL_COLOR);
  EXPECT_EQ(label->anchor,
            sf::Vector2i(FIRST_RAIL_TAB_LEFT, RAIL_TOP_MIDDLE.y));
}

/**
 * Given the rail of the tests, ONE open
 * When it is painted
 * Then TWO, neither open nor locked, is written in pale blue
 */
TEST(TabRailTest, AClosedTabIsWrittenInPaleBlue) {
  const InterfaceCanvasRecord painting = railPaintingOf(threeTabRail());
  const RecordedWriting* label = writingOf(painting, SECOND_RAIL_LABEL);

  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->style.color, IDLE_LABEL_COLOR);
}

/**
 * Given the rail of the tests, LOCK locked
 * When it is painted
 * Then LOCK is written in blue
 */
TEST(TabRailTest, ALockedTabIsWrittenInBlue) {
  const InterfaceCanvasRecord painting = railPaintingOf(threeTabRail());
  const RecordedWriting* label = writingOf(painting, LOCKED_RAIL_LABEL);

  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->style.color, LOCKED_LABEL_COLOR);
}

/**
 * Given the rail of the tests, LOCK locked
 * When it is painted
 * Then the SOON badge is written after it
 */
TEST(TabRailTest, ALockedTabCarriesTheSoonBadge) {
  const InterfaceCanvasRecord painting = railPaintingOf(threeTabRail());

  EXPECT_NE(writingOf(painting, LOCKED_BADGE), nullptr);
}

/**
 * Given the rail of the tests, ONE open from x 164 to 188
 * When it is painted
 * Then every cyan fill, the bar and its dot, lies under ONE
 */
TEST(TabRailTest, TheBarUnderlinesTheOpenTabOnly) {
  const InterfaceCanvasRecord painting = railPaintingOf(threeTabRail());
  int cyanFills = 0;
  for (const RecordedFill& fill : painting.fills) {
    if (fill.color != INDICATOR_COLOR) {
      continue;
    }
    ++cyanFills;
    EXPECT_GE(fill.area.position.x, FIRST_RAIL_TAB_LEFT);
    EXPECT_LE(fill.area.position.x + fill.area.size.x, FIRST_RAIL_TAB_RIGHT);
  }
  EXPECT_GT(cyanFills, 0);
}

/**
 * Given the rail of the tests
 * When the pointer lies inside TWO
 * Then TWO is the tab under it
 */
TEST(TabRailTest, FindsTheTabUnderThePointer) {
  EXPECT_EQ(threeTabRail().tabAt(IN_THE_SECOND_RAIL_TAB), SECOND_RAIL_TAB);
}

/**
 * Given the rail of the tests
 * When the pointer lies in the gap between ONE and TWO
 * Then no tab is under it
 */
TEST(TabRailTest, FindsNoTabBetweenTwoTabs) {
  EXPECT_FALSE(
      threeTabRail().tabAt(BETWEEN_THE_FIRST_TWO_RAIL_TABS).has_value());
}

/**
 * Given the rail of the tests
 * When the pointer lies under the row, past its bar
 * Then no tab is under it
 */
TEST(TabRailTest, FindsNoTabUnderTheRow) {
  EXPECT_FALSE(threeTabRail().tabAt(UNDER_THE_RAIL).has_value());
}

/**
 * Given the rail of the tests, TWO closed
 * When the pointer moves over TWO
 * Then TWO is written in star white
 */
TEST(TabRailTest, ATabUnderThePointerLightsUp) {
  TabRail rail = threeTabRail();
  rail.hover(SECOND_RAIL_TAB);

  const InterfaceCanvasRecord painting = railPaintingOf(rail);
  const RecordedWriting* label = writingOf(painting, SECOND_RAIL_LABEL);

  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->style.color, OPEN_LABEL_COLOR);
}

/**
 * Given the rail of the tests, LOCK locked
 * When it is refused, and a moment passes
 * Then LOCK is written away from where it stands
 */
TEST(TabRailTest, ARefusedTabShakes) {
  const InterfaceCanvasRecord still = railPaintingOf(threeTabRail());
  TabRail rail = threeTabRail();
  rail.refuse(LOCKED_RAIL_TAB);
  rail.advance(INSIDE_THE_SHAKE);

  const InterfaceCanvasRecord shaken = railPaintingOf(rail);

  EXPECT_NE(anchorOf(shaken, LOCKED_RAIL_LABEL),
            anchorOf(still, LOCKED_RAIL_LABEL));
}

/**
 * Given the rail of the tests, LOCK refused
 * When the shake has ended
 * Then LOCK is written back where it stands
 */
TEST(TabRailTest, ARefusedTabComesBackOnceTheShakeEnds) {
  const InterfaceCanvasRecord still = railPaintingOf(threeTabRail());
  TabRail rail = threeTabRail();
  rail.refuse(LOCKED_RAIL_TAB);
  rail.advance(PAST_THE_SHAKE);

  const InterfaceCanvasRecord settled = railPaintingOf(rail);

  EXPECT_EQ(anchorOf(settled, LOCKED_RAIL_LABEL),
            anchorOf(still, LOCKED_RAIL_LABEL));
}
