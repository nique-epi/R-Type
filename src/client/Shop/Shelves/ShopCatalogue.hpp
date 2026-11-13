#pragma once

#include "ShopWares.hpp"

namespace rtype::client {

/** @returns What the shop sells: every accessory of the catalog that has a
 * price, in its order, and every hull that has one, the cheapest first,
 * those of one price in the order of the catalog. */
[[nodiscard]] ShopWares waresForSale();

}  // namespace rtype::client
