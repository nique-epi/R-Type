#include <gtest/gtest.h>
#include <optional>
#include "TabStripWidthFixtures.hpp"
#include "TabStripWidthTestConstants.hpp"

using rtype::client::testing::IN_THE_SECOND_TAB;
using rtype::client::testing::LEFT_OF_THE_SECOND_TAB;
using rtype::client::testing::ON_THE_NEXT_KEYCAP;
using rtype::client::testing::RIGHT_OF_THE_STRIP;
using rtype::client::testing::SECOND_TAB;

/**
 * Given a strip of two tabs, 320 pixels wide
 * When the first pixel of its second tab is clicked
 * Then that tab opens
 */
TEST(TabStripWidth, TabsShareTheWidthGiven) {
  EXPECT_EQ(narrowStrip().tabOpenedAt(IN_THE_SECOND_TAB), SECOND_TAB);
}

/**
 * Given a strip of two tabs, 320 pixels wide
 * When the pixel just left of its second tab is clicked
 * Then no tab opens: it lies in the gap between the two
 */
TEST(TabStripWidth, TheGapStaysBetweenTheTabs) {
  EXPECT_EQ(narrowStrip().tabOpenedAt(LEFT_OF_THE_SECOND_TAB), std::nullopt);
}

/**
 * Given a strip of two tabs, 320 pixels wide
 * When the first pixel of the keycap of the next tab is clicked
 * Then the next tab opens: the keycap moved with the width
 */
TEST(TabStripWidth, TheNextKeycapEndsTheStrip) {
  EXPECT_EQ(narrowStrip().tabOpenedAt(ON_THE_NEXT_KEYCAP), SECOND_TAB);
}

/**
 * Given a strip of two tabs, 320 pixels wide
 * When the pixel just right of the strip is clicked
 * Then no tab opens
 */
TEST(TabStripWidth, NothingOpensPastTheWidth) {
  EXPECT_EQ(narrowStrip().tabOpenedAt(RIGHT_OF_THE_STRIP), std::nullopt);
}
