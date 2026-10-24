#include <gtest/gtest.h>
#include "BackgroundColorConstants.hpp"
#include "KeyCap.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::KeyCap;
using rtype::client::STAR_GOLD;

/**
 * Given a one-letter key and the space bar
 * When their keycaps are measured
 * Then Q keeps the 14 pixels of the mockup and SPACE widens to its name
 */
TEST(KeyCapTest, AKeyCapIsAtLeastFourteenPixelsWide) {
  EXPECT_EQ(KeyCap("Q", KeyCap::Edge::Plain).width(), 14);
  EXPECT_EQ(KeyCap("SPACE", KeyCap::Edge::Plain).width(), 46);
}

/**
 * Given the keycap of a one-letter key at the origin
 * When it is painted
 * Then its letter is centered, 3 pixels in
 */
TEST(KeyCapTest, ItsNameIsCentered) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  KeyCap("Q", KeyCap::Edge::Plain).paint(canvas, {0, 0});
  ASSERT_EQ(record.writings.size(), 1U);
  EXPECT_EQ(record.writings.front().anchor.x, 3);
}

/**
 * Given the keycap of a key two actions share
 * When it is painted
 * Then its edge is gold
 */
TEST(KeyCapTest, AKeySharedByTwoActionsHasAGoldEdge) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  KeyCap("Q", KeyCap::Edge::Conflict).paint(canvas, {0, 0});
  ASSERT_FALSE(record.fills.empty());
  EXPECT_EQ(record.fills.front().color, STAR_GOLD);
}
