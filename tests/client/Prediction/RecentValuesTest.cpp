#include <gtest/gtest.h>
#include <vector>
#include "RecentValues.hpp"
#include "RecentValuesTestConstants.hpp"

using rtype::client::RecentValues;
using rtype::client::testing::FIRST_PUSHED;
using rtype::client::testing::FOURTH_PUSHED;
using rtype::client::testing::SECOND_PUSHED;
using rtype::client::testing::THIRD_PUSHED;

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

  window.push(FIRST_PUSHED);
  window.push(SECOND_PUSHED);

  EXPECT_EQ(std::vector<int>(window.values().begin(), window.values().end()),
            (std::vector<int>{FIRST_PUSHED, SECOND_PUSHED}));
}

/**
 * Given a window of three values that is full
 * When a fourth value is pushed
 * Then the oldest one is forgotten and the three newest remain, oldest first
 */
TEST(RecentValues, ForgetsTheOldestValueOnceFull) {
  RecentValues<int, 3> window;
  window.push(FIRST_PUSHED);
  window.push(SECOND_PUSHED);
  window.push(THIRD_PUSHED);

  window.push(FOURTH_PUSHED);

  EXPECT_EQ(std::vector<int>(window.values().begin(), window.values().end()),
            (std::vector<int>{SECOND_PUSHED, THIRD_PUSHED, FOURTH_PUSHED}));
}
