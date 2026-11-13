#include <gtest/gtest.h>
#include <cstddef>
#include "LookItemPurchase.hpp"
#include "LookItemPurchaseFixtures.hpp"
#include "LookItemPurchaseTestConstants.hpp"
#include "NamedPrice.hpp"
#include "Purchase.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"
#include "ShipLookConstants.hpp"

using rtype::client::buyLookItem;
using rtype::client::lookItemPrice;
using rtype::client::Purchase;
using rtype::client::PurchaseRefusal;
using rtype::client::Purse;
using rtype::client::testing::BALANCE_AFTER_THE_CROWN;
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
 * When it buys the crown, 600 kopecks
 * Then 400 kopecks are left and the locker holds the crown
 */
TEST(LookItemPurchase, BuyingTakesThePriceAndFillsTheLocker) {
  const Purchase bought =
      buyLookItem(purseWithAnEmptyLocker(RICH_BALANCE), itemNamed(CROWN_NAME));

  EXPECT_FALSE(bought.refusal.has_value());
  EXPECT_EQ(bought.purse.kopecks, BALANCE_AFTER_THE_CROWN);
  EXPECT_TRUE(bought.purse.locker.items.test(itemNamed(CROWN_NAME)));
}

/**
 * Given a purse of exactly the price of the crown
 * When it buys the crown
 * Then the crown is bought and nothing is left
 */
TEST(LookItemPurchase, TheExactPriceBuys) {
  const Purchase bought = buyLookItem(purseWithAnEmptyLocker(CROWN_PRICE_PAID),
                                      itemNamed(CROWN_NAME));

  EXPECT_FALSE(bought.refusal.has_value());
  EXPECT_EQ(bought.purse.kopecks, 0U);
}

/**
 * Given a purse one kopeck short of the crown
 * When it buys the crown
 * Then the shop refuses, too expensive, and the purse is as it was
 */
TEST(LookItemPurchase, AnItemAboveTheBalanceIsTooExpensive) {
  const Purse purse = purseWithAnEmptyLocker(ONE_KOPECK_SHORT_OF_THE_CROWN);

  const Purchase refused = buyLookItem(purse, itemNamed(CROWN_NAME));

  EXPECT_EQ(refused.refusal, PurchaseRefusal::TooExpensive);
  EXPECT_EQ(refused.purse, purse);
}

/**
 * Given a rich purse whose locker already holds the top hat
 * When it buys the top hat
 * Then the shop refuses, already owned, and takes nothing
 */
TEST(LookItemPurchase, AnItemHeldIsNotSoldAgain) {
  const Purse purse{.kopecks = RICH_BALANCE, .locker = {}};

  const Purchase refused = buyLookItem(purse, itemNamed(TOP_HAT_NAME));

  EXPECT_EQ(refused.refusal, PurchaseRefusal::AlreadyOwned);
  EXPECT_EQ(refused.purse, purse);
}

/**
 * Given a rich purse whose locker lost the top hat, an item of the starter
 * kit, which has no price
 * When it buys the top hat
 * Then the shop refuses: it is not for sale
 */
TEST(LookItemPurchase, AStarterItemIsNotForSale) {
  const Purchase refused = buyLookItem(purseWithAnEmptyLocker(RICH_BALANCE),
                                       itemNamed(TOP_HAT_NAME));

  EXPECT_EQ(refused.refusal, PurchaseRefusal::NotForSale);
}
