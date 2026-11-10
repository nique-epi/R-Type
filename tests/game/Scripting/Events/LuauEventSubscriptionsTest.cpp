#include <gtest/gtest.h>
#include <string>
#include "LuauEventSubscriptions.hpp"
#include "LuauEventSubscriptionsTestConstants.hpp"

using rtype::game::LuauEventSubscriptions;

/**
 * Given the subscriptions of an entity, one of whose functions failed
 * When they are asked why
 * Then they give the reason of that failure
 */
TEST(LuauEventSubscriptions, KeepWhyAFunctionFailed) {
  LuauEventSubscriptions subscriptions;

  subscriptions.fail(std::string{REASON_OF_A_FAILURE});

  EXPECT_EQ(subscriptions.failure().value_or(std::string{}),
            REASON_OF_A_FAILURE);
}

/**
 * Given the subscriptions of an entity, one of whose functions failed
 * When they are cleared
 * Then they no longer hold a failure
 */
TEST(LuauEventSubscriptions, ForgetAFailureOnceCleared) {
  LuauEventSubscriptions subscriptions;
  subscriptions.fail(std::string{REASON_OF_A_FAILURE});

  subscriptions.clear();

  EXPECT_FALSE(subscriptions.failure().has_value());
}
