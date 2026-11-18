#include <gtest/gtest.h>
#include "Kopecks.hpp"
#include "KopecksTestConstants.hpp"

using rtype::client::kopecksText;
using rtype::client::testing::FOUR_DIGIT_AMOUNT;
using rtype::client::testing::FOUR_DIGITS_WRITTEN;
using rtype::client::testing::LARGEST_BALANCE;
using rtype::client::testing::LARGEST_BALANCE_WRITTEN;
using rtype::client::testing::THREE_DIGIT_AMOUNT;
using rtype::client::testing::THREE_DIGITS_WRITTEN;

/**
 * Given amounts of three, four and six digits
 * When they are written
 * Then a space parts their thousands, and none starts a short amount
 */
TEST(Kopecks, ASpacePartsTheThousands) {
  EXPECT_EQ(kopecksText(THREE_DIGIT_AMOUNT), THREE_DIGITS_WRITTEN);
  EXPECT_EQ(kopecksText(FOUR_DIGIT_AMOUNT), FOUR_DIGITS_WRITTEN);
  EXPECT_EQ(kopecksText(LARGEST_BALANCE), LARGEST_BALANCE_WRITTEN);
}
