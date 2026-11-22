#include <gtest/gtest.h>
#include "Language.hpp"
#include "PurchaseRefusal.hpp"
#include "ShopScreenTestConstants.hpp"
#include "ShopStatus.hpp"

using rtype::client::Language;
using rtype::client::PurchaseRefusal;
using rtype::client::refusalLineOf;
using rtype::client::testing::ALREADY_YOURS_LINE;
using rtype::client::testing::FRENCH_NOT_ENOUGH_LINE;
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

/**
 * Given the client in French and a purchase refused for 150 kopecks missing
 * When its status line is asked
 * Then it names the kopecks missing in French, the number at its end
 */
TEST(ShopStatus, ATooExpensiveWareSaysItInFrench) {
  EXPECT_EQ(refusalLineOf(PurchaseRefusal::TooExpensive, KOPECKS_MISSING,
                          Language::French),
            FRENCH_NOT_ENOUGH_LINE);
}
