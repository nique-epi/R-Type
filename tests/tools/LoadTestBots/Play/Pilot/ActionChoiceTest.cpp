#include <gtest/gtest.h>
#include "ActionChoice.hpp"
#include "ActionChoiceTestConstants.hpp"
#include "ActionSet.hpp"
#include "PlayerAction.hpp"

using rtype::client::ActionSet;
using rtype::client::PlayerAction;
using rtype::tools::actionSetOf;
using rtype::tools::testing::FIRE_ALONE;
using rtype::tools::testing::LAST_CHOICE;
using rtype::tools::testing::NOTHING_HELD;

/**
 * Given the first choice
 * When its actions are asked for
 * Then nothing is held
 */
TEST(ActionChoice, TheFirstChoiceHoldsNothing) {
  EXPECT_EQ(actionSetOf(NOTHING_HELD).bits(), ActionSet{}.bits());
}

/**
 * Given the choice of the fire alone, the first of the second half
 * When its actions are asked for
 * Then only the fire is held
 */
TEST(ActionChoice, TheSecondHalfAddsTheFire) {
  ActionSet expected;
  expected.add(PlayerAction::Shoot);

  EXPECT_EQ(actionSetOf(FIRE_ALONE).bits(), expected.bits());
}

/**
 * Given the last choice
 * When its actions are asked for
 * Then a diagonal is held with the fire
 */
TEST(ActionChoice, TheLastChoiceHoldsADiagonalAndTheFire) {
  ActionSet expected;
  expected.add(PlayerAction::Down);
  expected.add(PlayerAction::Right);
  expected.add(PlayerAction::Shoot);

  EXPECT_EQ(actionSetOf(LAST_CHOICE).bits(), expected.bits());
}
