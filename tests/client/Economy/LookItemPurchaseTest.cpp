#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "LookItemPurchase.hpp"
#include "LookItemPurchaseFixtures.hpp"
#include "LookItemPurchaseTestConstants.hpp"
#include "LookItems.hpp"
#include "NamedPrice.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"
#include "ShipLookConstants.hpp"

using rtype::client::lookRefusalOf;
using rtype::client::PurchaseRefusal;
using rtype::client::Purse;
using rtype::client::testing::CATALOGUE_PRICES;
using rtype::client::testing::CROWN_NAME;
using rtype::client::testing::CROWN_PRICE_PAID;
using rtype::client::testing::itemNamed;
using rtype::client::testing::NamedPrice;
using rtype::client::testing::ONE_KOPECK_SHORT_OF_THE_CROWN;
using rtype::client::testing::purseWithAnEmptyLocker;
using rtype::client::testing::RICH_BALANCE;
using rtype::client::testing::TOP_HAT_NAME;
using rtype::identifiers::SHIP_LOOK_ITEM_COUNT;
using rtype::network::lookItemPrice;

/**
 * Given the catalogue of thirty items
 * When the price of each is read by its name
 * Then it is the price the shop was decided with, none for the starter kit
 */
TEST(LookItemPurchase, EveryItemHasItsDecidedPrice) {
  ASSERT_EQ(CATALOGUE_PRICES.size(), SHIP_LOOK_ITEM_COUNT);
  for (const NamedPrice& expected : CATALOGUE_PRICES) {
    SCOPED_TRACE(expected.name);
    const std::size_t item = itemNamed(expected.name);
    ASSERT_LT(item, SHIP_LOOK_ITEM_COUNT);
    EXPECT_EQ(lookItemPrice(item), expected.price);
  }
}

/**
 * Given a purse of 1000 kopecks that does not hold the crown
 * When it tries the crown, 600 kopecks
 * Then nothing refuses it
 */
TEST(LookItemPurchase, AnItemTheBalancePaysIsRefusedNothing) {
  EXPECT_EQ(lookRefusalOf(purseWithAnEmptyLocker(RICH_BALANCE),
                          itemNamed(CROWN_NAME)),
            std::nullopt);
}

/**
 * Given a purse of exactly the price of the crown
 * When it tries the crown
 * Then nothing refuses it
 */
TEST(LookItemPurchase, TheExactPriceBuys) {
  EXPECT_EQ(lookRefusalOf(purseWithAnEmptyLocker(CROWN_PRICE_PAID),
                          itemNamed(CROWN_NAME)),
            std::nullopt);
}

/**
 * Given a purse one kopeck short of the crown
 * When it buys the crown
 * Then the shop refuses, too expensive
 */
TEST(LookItemPurchase, AnItemAboveTheBalanceIsTooExpensive) {
  const Purse purse = purseWithAnEmptyLocker(ONE_KOPECK_SHORT_OF_THE_CROWN);

  EXPECT_EQ(lookRefusalOf(purse, itemNamed(CROWN_NAME)),
            PurchaseRefusal::TooExpensive);
}

/**
 * Given a rich purse whose locker already holds the top hat
 * When it buys the top hat
 * Then the shop refuses, already owned
 */
TEST(LookItemPurchase, AnItemHeldIsNotSoldAgain) {
  const Purse purse{.kopecks = RICH_BALANCE, .locker = {}};

  EXPECT_EQ(lookRefusalOf(purse, itemNamed(TOP_HAT_NAME)),
            PurchaseRefusal::AlreadyOwned);
}

/**
 * Given a rich purse whose locker lost the top hat, an item of the starter
 * kit, which has no price
 * When it buys the top hat
 * Then the shop refuses: it is not for sale
 */
TEST(LookItemPurchase, AStarterItemIsNotForSale) {
  EXPECT_EQ(lookRefusalOf(purseWithAnEmptyLocker(RICH_BALANCE),
                          itemNamed(TOP_HAT_NAME)),
            PurchaseRefusal::NotForSale);
}
