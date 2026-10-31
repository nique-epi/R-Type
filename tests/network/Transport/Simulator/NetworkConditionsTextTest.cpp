#include <gtest/gtest.h>
#include "NetworkConditionsText.hpp"
#include "NetworkConditionsTextTestConstants.hpp"
#include "NetworkSimulatorTestConstants.hpp"

/**
 * Given conditions whose five values all differ
 * When they are written in words
 * Then each value is in its place, with its unit
 */
TEST(NetworkConditionsText, NamesEachValueInItsPlace) {
  EXPECT_EQ(rtype::network::conditionsText(BAD_OF_THE_TABLE),
            BAD_NETWORK_IN_WORDS);
}
