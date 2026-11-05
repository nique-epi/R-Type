#include <gtest/gtest.h>
#include <cstdint>
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
  std::vector<std::uint16_t> given;
  given.reserve(DEMONSTRATION_MESSAGE_COUNT);
  for (std::uint16_t step = 0; step < DEMONSTRATION_MESSAGE_COUNT; ++step) {
    given.push_back(step);
  }

  runTheDemonstration();

  EXPECT_EQ(delivered(), given);
}

}  // namespace rtype::network::testing
