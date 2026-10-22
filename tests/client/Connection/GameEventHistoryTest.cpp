#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include "GameEventHistory.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::GameEventHistory;
using rtype::engine::Duration;

namespace {

constexpr std::uint16_t EVENT_NUMBER = 42;
constexpr Duration COPY_GAP = std::chrono::milliseconds(50);
constexpr Duration EVENT_MEMORY_REQUIRED = std::chrono::seconds(1);

}  // namespace

/**
 * Given an event already seen
 * When its next two copies come 50 ms apart
 * Then neither is a first copy
 */
TEST(GameEventHistory, TheOtherCopiesOfAnEventAreNotNew) {
  GameEventHistory history;
  ASSERT_TRUE(history.isFirstCopy(EVENT_NUMBER));

  history.advance(COPY_GAP);
  const bool second = history.isFirstCopy(EVENT_NUMBER);
  history.advance(COPY_GAP);
  const bool third = history.isFirstCopy(EVENT_NUMBER);

  EXPECT_FALSE(second);
  EXPECT_FALSE(third);
}

/**
 * Given an event seen
 * When its number comes back one nanosecond less than a second later
 * Then it is still a copy
 */
TEST(GameEventHistory, ANumberIsRememberedForASecond) {
  GameEventHistory history;
  ASSERT_TRUE(history.isFirstCopy(EVENT_NUMBER));

  history.advance(EVENT_MEMORY_REQUIRED - ONE_NANOSECOND);

  EXPECT_FALSE(history.isFirstCopy(EVENT_NUMBER));
}

/**
 * Given an event seen
 * When its number comes back a second later
 * Then it is new: another event, after the numbers went round
 */
TEST(GameEventHistory, ANumberIsForgottenAfterASecond) {
  GameEventHistory history;
  ASSERT_TRUE(history.isFirstCopy(EVENT_NUMBER));

  history.advance(EVENT_MEMORY_REQUIRED);

  EXPECT_TRUE(history.isFirstCopy(EVENT_NUMBER));
}

/**
 * Given an event seen in a session left
 * When the history is forgotten and the number comes again
 * Then it is new
 */
TEST(GameEventHistory, ForgettingClearsEveryNumber) {
  GameEventHistory history;
  ASSERT_TRUE(history.isFirstCopy(EVENT_NUMBER));

  history.forget();

  EXPECT_TRUE(history.isFirstCopy(EVENT_NUMBER));
}
