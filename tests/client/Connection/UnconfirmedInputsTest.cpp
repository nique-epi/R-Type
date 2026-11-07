#include <gtest/gtest.h>
#include <cstdint>
#include <tuple>
#include <vector>
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"
#include "UnconfirmedInputs.hpp"
#include "UnconfirmedInputsTestConstants.hpp"

namespace rtype::client {

using testing::ACTION_BITS_MASK;
using testing::DOWN_ACTION;
using testing::FAR_BEYOND_THE_NEWEST_INPUT;
using testing::FIFTH_INPUT;
using testing::LEFT_ACTION;
using testing::NINETEENTH_INPUT;
using testing::REPEATED_INPUT_LIMIT;
using testing::SHOOT_ACTION;
using testing::THIRD_INPUT;
using testing::TWENTY_INPUTS;
using testing::UP_ACTION;

/**
 * Given nothing sent yet
 * When the first input is added
 * Then it is numbered 1 and repeats nothing
 */
TEST(UnconfirmedInputs, NumbersTheFirstInputOneAndRepeatsNothing) {
  UnconfirmedInputs inputs;

  const network::PlayerInput first =
      inputs.add(UP_ACTION, network::NO_SHOWN_TICK);

  EXPECT_EQ(first,
            (network::PlayerInput{
                .inputNumber = 1, .actions = UP_ACTION, .earlierActions = {}}));
}

/**
 * Given three inputs added and none confirmed
 * When the third is added
 * Then it repeats the second then the first
 */
TEST(UnconfirmedInputs, RepeatsTheEarlierInputsNewestFirst) {
  UnconfirmedInputs inputs;
  std::ignore = inputs.add(UP_ACTION, network::NO_SHOWN_TICK);
  std::ignore = inputs.add(DOWN_ACTION, network::NO_SHOWN_TICK);

  const network::PlayerInput third =
      inputs.add(LEFT_ACTION, network::NO_SHOWN_TICK);

  EXPECT_EQ(third,
            (network::PlayerInput{.inputNumber = THIRD_INPUT,
                                  .actions = LEFT_ACTION,
                                  .earlierActions = {DOWN_ACTION, UP_ACTION}}));
}

/**
 * Given twenty inputs added and none confirmed
 * When the twentieth is added
 * Then it repeats the fifteen inputs before it, numbers 19 down to 5
 */
TEST(UnconfirmedInputs, RepeatsAtMostTheLimitOfEarlierInputs) {
  UnconfirmedInputs inputs;
  network::PlayerInput last;
  for (std::uint32_t number = 1; number <= TWENTY_INPUTS; ++number) {
    last = inputs.add(static_cast<std::uint8_t>(number & ACTION_BITS_MASK),
                      network::NO_SHOWN_TICK);
  }

  ASSERT_EQ(last.earlierActions.size(), REPEATED_INPUT_LIMIT);
  EXPECT_EQ(last.earlierActions.front(), NINETEENTH_INPUT & ACTION_BITS_MASK);
  EXPECT_EQ(last.earlierActions.back(), FIFTH_INPUT & ACTION_BITS_MASK);
}

/**
 * Given five inputs added, the server confirming up to the third
 * When the sixth is added
 * Then it repeats only the fifth and the fourth
 */
TEST(UnconfirmedInputs, DoesNotRepeatAConfirmedInput) {
  UnconfirmedInputs inputs;
  for (std::uint32_t number = 1; number <= FIFTH_INPUT; ++number) {
    std::ignore =
        inputs.add(static_cast<std::uint8_t>(number), network::NO_SHOWN_TICK);
  }
  inputs.confirmUpTo(THIRD_INPUT);

  const network::PlayerInput sixth =
      inputs.add(SHOOT_ACTION, network::NO_SHOWN_TICK);

  EXPECT_EQ(sixth.earlierActions, (std::vector<std::uint8_t>{5, 4}));
}

/**
 * Given three inputs added, the server confirming up to the third, and a
 * confirmation far beyond the newest after it
 * When the next input is added
 * Then it repeats nothing and is numbered 4
 */
TEST(UnconfirmedInputs, RepeatsNothingOnceEveryInputIsConfirmed) {
  UnconfirmedInputs inputs;
  for (std::uint32_t number = 1; number <= THIRD_INPUT; ++number) {
    std::ignore = inputs.add(UP_ACTION, network::NO_SHOWN_TICK);
  }
  inputs.confirmUpTo(THIRD_INPUT);
  inputs.confirmUpTo(FAR_BEYOND_THE_NEWEST_INPUT);

  const network::PlayerInput fourth =
      inputs.add(UP_ACTION, network::NO_SHOWN_TICK);

  EXPECT_EQ(fourth, (network::PlayerInput{.inputNumber = THIRD_INPUT + 1,
                                          .actions = UP_ACTION,
                                          .earlierActions = {}}));
}

/**
 * Given two inputs added
 * When the server confirms up to 0, which it has not confirmed anything yet
 * Then the next input still repeats both
 */
TEST(UnconfirmedInputs, KeepsEveryInputWhileNothingIsConfirmed) {
  UnconfirmedInputs inputs;
  std::ignore = inputs.add(UP_ACTION, network::NO_SHOWN_TICK);
  std::ignore = inputs.add(DOWN_ACTION, network::NO_SHOWN_TICK);
  inputs.confirmUpTo(0);

  const network::PlayerInput third =
      inputs.add(LEFT_ACTION, network::NO_SHOWN_TICK);

  EXPECT_EQ(third.earlierActions,
            (std::vector<std::uint8_t>{DOWN_ACTION, UP_ACTION}));
}

/**
 * Given two inputs added
 * When the inputs are cleared and one is added
 * Then it is numbered 1 and repeats nothing
 */
TEST(UnconfirmedInputs, StartsAgainFromTheFirstInputWhenCleared) {
  UnconfirmedInputs inputs;
  std::ignore = inputs.add(UP_ACTION, network::NO_SHOWN_TICK);
  std::ignore = inputs.add(DOWN_ACTION, network::NO_SHOWN_TICK);
  inputs.clear();

  const network::PlayerInput first =
      inputs.add(LEFT_ACTION, network::NO_SHOWN_TICK);

  EXPECT_EQ(first, (network::PlayerInput{.inputNumber = 1,
                                         .actions = LEFT_ACTION,
                                         .earlierActions = {}}));
}

}  // namespace rtype::client
