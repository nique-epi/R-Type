#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include "LookItemPurchase.hpp"
#include "ShopCatalogue.hpp"
#include "ShopCatalogueTestConstants.hpp"
#include "ShopWares.hpp"

using rtype::client::lookItemPrice;
using rtype::client::ShopWares;
using rtype::client::waresForSale;
using rtype::client::testing::HULLS_FOR_SALE;
using rtype::client::testing::LOOKS_FOR_SALE;
using rtype::client::testing::TOP_HAT_ITEM;
using ::testing::Contains;
using ::testing::Not;

/**
 * Given the catalogue of the items and their prices
 * When the wares of the shop are listed
 * Then the locker sells the 22 looks that have a price, and the case the 5
 * hulls
 */
TEST(ShopCatalogue, TheShopSellsEveryPricedLookAndHull) {
  const ShopWares wares = waresForSale();

  EXPECT_EQ(wares.looks.size(), LOOKS_FOR_SALE);
  EXPECT_EQ(wares.hulls.size(), HULLS_FOR_SALE);
}

/**
 * Given the top hat, an item of the starter kit, which has no price
 * When the wares of the shop are listed
 * Then the locker does not sell it
 */
TEST(ShopCatalogue, AStarterItemIsNotForSale) {
  EXPECT_THAT(waresForSale().looks, Not(Contains(TOP_HAT_ITEM)));
}

/**
 * Given the hulls of the catalogue
 * When the wares of the shop are listed
 * Then the case shows them by rising price
 */
TEST(ShopCatalogue, TheHullsRiseInPrice) {
  const ShopWares wares = waresForSale();

  EXPECT_TRUE(std::ranges::is_sorted(wares.hulls, {}, lookItemPrice));
}
