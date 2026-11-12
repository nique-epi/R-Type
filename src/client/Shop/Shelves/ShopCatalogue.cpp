#include "ShopCatalogue.hpp"
#include <algorithm>
#include <cstddef>
#include "EconomyConstants.hpp"
#include "LookItemPurchase.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookNumbers.hpp"
#include "ShopWares.hpp"

namespace rtype::client {

ShopWares waresForSale() {
  ShopWares wares;
  for (std::size_t item = 0; item < identifiers::SHIP_LOOK_ITEM_COUNT; ++item) {
    if (lookItemPrice(item) == NOT_FOR_SALE) {
      continue;
    }
    if (identifiers::isHullItem(item)) {
      wares.hulls.push_back(item);
    } else {
      wares.looks.push_back(item);
    }
  }
  std::ranges::stable_sort(wares.hulls, {}, lookItemPrice);
  return wares;
}

}  // namespace rtype::client
