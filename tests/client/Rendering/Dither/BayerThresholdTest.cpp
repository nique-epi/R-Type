#include <gtest/gtest.h>
#include "BayerThreshold.hpp"
#include "BayerThresholdTestConstants.hpp"

using rtype::client::bayerThresholdAt;

/**
 * Given the pixel at the origin, where the matrix holds its lowest level
 * When its threshold is read
 * Then it is half a level out of sixteen
 */
TEST(BayerThreshold, TheOriginHasTheLowestThreshold) {
  EXPECT_DOUBLE_EQ(bayerThresholdAt(CORNER_OF_THE_MATRIX), LOWEST_THRESHOLD);
}
