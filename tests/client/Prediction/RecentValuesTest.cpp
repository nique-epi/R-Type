#include <gtest/gtest.h>
#include <vector>
#include "RecentValues.hpp"

using rtype::client::RecentValues;

/**
 * Given a window of three values that holds nothing
 * When its values are read
 * Then there are none
 */
TEST(RecentValues, HoldsNothingBeforeAPush) {
  const RecentValues<int, 3> window;

  EXPECT_TRUE(window.values().empty());
}

/**
 * Given a window of three values
 * When two values are pushed
 * Then it holds both, the oldest first
 */
TEST(RecentValues, KeepsWhatWasPushedOldestFirst) {
  RecentValues<int, 3> window;

  window.push(7);
  window.push(8);

  EXPECT_EQ(std::vector<int>(window.values().begin(), window.values().end()),
            (std::vector<int>{7, 8}));
}

/**
 * Given a window of three values that is full
 * When a fourth value is pushed
 * Then the oldest one is forgotten and the three newest remain, oldest first
 */
TEST(RecentValues, ForgetsTheOldestValueOnceFull) {
  RecentValues<int, 3> window;
  window.push(7);
  window.push(8);
  window.push(9);

  window.push(10);

  EXPECT_EQ(std::vector<int>(window.values().begin(), window.values().end()),
            (std::vector<int>{8, 9, 10}));
}
