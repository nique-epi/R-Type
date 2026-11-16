#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "LookItems.hpp"
#include "LookItemsTestConstants.hpp"

using rtype::network::lookItemOfName;
using rtype::network::testing::CROWN_IN_CAPITALS;
using rtype::network::testing::CROWN_ITEM;
using rtype::network::testing::CROWN_ITEM_NAME;
using rtype::network::testing::NO_ITEM_NAME;

/**
 * Given the name of the crown, as the shop writes it in a path
 * When the item of that name is looked up
 * Then it is the third item of the catalogue
 */
TEST(LookItems, FindsAnItemByItsName) {
  EXPECT_EQ(lookItemOfName(CROWN_ITEM_NAME),
            std::optional<std::size_t>{CROWN_ITEM});
}

/**
 * Given the name of the crown in capitals, and an empty name
 * When the item of each is looked up
 * Then none is found: a name is compared byte for byte
 */
TEST(LookItems, FindsNoItemForANameNoItemHas) {
  EXPECT_FALSE(lookItemOfName(CROWN_IN_CAPITALS).has_value());
  EXPECT_FALSE(lookItemOfName(NO_ITEM_NAME).has_value());
}
