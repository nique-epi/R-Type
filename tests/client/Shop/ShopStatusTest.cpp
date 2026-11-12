#include <gtest/gtest.h>
#include "PurchaseRefusal.hpp"
#include "ShopScreenTestConstants.hpp"
#include "ShopStatus.hpp"

using rtype::client::PurchaseRefusal;
using rtype::client::refusalLineOf;
using rtype::client::testing::ALREADY_YOURS_LINE;
using rtype::client::testing::KOPECKS_MISSING;
using rtype::client::testing::NOT_ENOUGH_LINE;

/**
 * Given a purchase refused because the player lacks 150 kopecks
 * When its status line is asked
 * Then it names the kopecks and how many are missing
 */
TEST(ShopStatus, ATooExpensiveWareSaysHowManyKopecksAreMissing) {
  EXPECT_EQ(refusalLineOf(PurchaseRefusal::TooExpensive, KOPECKS_MISSING),
            NOT_ENOUGH_LINE);
}

/**
 * Given a purchase refused because the player owns the ware
 * When its status line is asked
 * Then it says the ware is already theirs
 */
TEST(ShopStatus, AnOwnedWareSaysItIsAlreadyYours) {
  EXPECT_EQ(refusalLineOf(PurchaseRefusal::AlreadyOwned, 0),
            ALREADY_YOURS_LINE);
}
