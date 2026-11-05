#include <gtest/gtest.h>
#include <cstdint>
#include "InputRedundancyFixture.hpp"
#include "InputRedundancyTestConstants.hpp"

namespace rtype::server {

using testing::ACTION_BITS_MASK;
using testing::InputRedundancyFixture;
using testing::INPUTS_SENT;

/**
 * Given a network losing 10 percent of the datagrams in both directions, and a
 * player sending 2000 inputs
 * When the server reads what arrives
 * Then the game is shown every input from 1 to 2000, once each, in order, with
 * the actions the player held
 */
TEST_F(InputRedundancyFixture, TheServerSeesEveryInputThroughTenPercentLoss) {
  sendInputs(INPUTS_SENT);

  ASSERT_EQ(shown().size(), INPUTS_SENT);
  for (std::uint32_t index = 0; index < INPUTS_SENT; ++index) {
    EXPECT_EQ(shown()[index].input.inputNumber, index + 1);
    EXPECT_EQ(shown()[index].input.actions, (index + 1) & ACTION_BITS_MASK);
  }
}

}  // namespace rtype::server
