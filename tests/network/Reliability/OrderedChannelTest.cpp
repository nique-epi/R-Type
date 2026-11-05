#include <gtest/gtest.h>
#include <cstdint>
#include <numeric>
#include <vector>
#include "OrderedChannelFixture.hpp"
#include "ReliabilityTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given a link that loses one datagram in five each way and delays three in
 * ten
 * When DEMONSTRATION_MESSAGE_COUNT ordered messages are given, one per step,
 * and the steps go on until the last one lived its lifetime
 * Then the receiver handed over every one of them, once, in the order they
 * were given
 */
TEST_F(OrderedChannelFixture,
       DeliversEveryMessageInOrderThroughLossesAndDelays) {
  std::vector<std::uint16_t> given(DEMONSTRATION_MESSAGE_COUNT);
  std::iota(given.begin(), given.end(), std::uint16_t{0});

  runTheDemonstration();

  EXPECT_EQ(delivered(), given);
}

}  // namespace rtype::network::testing
