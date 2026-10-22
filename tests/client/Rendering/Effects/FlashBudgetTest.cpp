#include <gtest/gtest.h>
#include <chrono>
#include "EffectLengths.hpp"
#include "FlashBudget.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::FlashBudget;
using rtype::engine::Duration;

namespace {

constexpr Duration LOAN_TIME = std::chrono::milliseconds(100);

}  // namespace

/**
 * Given an empty budget
 * When effects ask for exactly the most pixels allowed, in two loans
 * Then both loans are made
 */
TEST(FlashBudget, LendsUpToTheMostPixelsAllowed) {
  FlashBudget budget;

  const bool first = budget.reserve(
      {.area = FLASH_PIXELS_ALLOWED - 1, .remaining = LOAN_TIME});
  const bool second = budget.reserve({.area = 1, .remaining = LOAN_TIME});

  EXPECT_TRUE(first);
  EXPECT_TRUE(second);
}

/**
 * Given a budget that lent the most pixels allowed
 * When an effect asks for one more pixel
 * Then it is refused, and nothing more is lent
 */
TEST(FlashBudget, RefusesAPixelPastTheMost) {
  FlashBudget budget;
  ASSERT_TRUE(
      budget.reserve({.area = FLASH_PIXELS_ALLOWED, .remaining = LOAN_TIME}));

  const bool lent = budget.reserve({.area = 1, .remaining = LOAN_TIME});

  EXPECT_FALSE(lent);
  EXPECT_EQ(budget.areaInUse(), FLASH_PIXELS_ALLOWED);
}

/**
 * Given every pixel lent for 100 ms
 * When one nanosecond less than 100 ms passes
 * Then the pixels are still lent
 */
TEST(FlashBudget, KeepsALoanUntilItsTimePassed) {
  FlashBudget budget;
  ASSERT_TRUE(
      budget.reserve({.area = FLASH_PIXELS_ALLOWED, .remaining = LOAN_TIME}));

  budget.advance(LOAN_TIME - ONE_TICK);

  EXPECT_EQ(budget.areaInUse(), FLASH_PIXELS_ALLOWED);
}

/**
 * Given every pixel lent for 100 ms
 * When 100 ms pass
 * Then another effect may take them all
 */
TEST(FlashBudget, GivesThePixelsBackOnceItsTimePassed) {
  FlashBudget budget;
  ASSERT_TRUE(
      budget.reserve({.area = FLASH_PIXELS_ALLOWED, .remaining = LOAN_TIME}));

  budget.advance(LOAN_TIME);

  EXPECT_TRUE(
      budget.reserve({.area = FLASH_PIXELS_ALLOWED, .remaining = LOAN_TIME}));
}
