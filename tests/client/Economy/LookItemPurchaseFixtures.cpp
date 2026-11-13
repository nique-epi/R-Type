#include "LookItemPurchaseFixtures.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Purse.hpp"
#include "ShipLookConstants.hpp"

namespace rtype::client::testing {

std::size_t itemNamed(std::string_view name) {
  return static_cast<std::size_t>(
      std::ranges::find(identifiers::SHIP_LOOK_ITEM_NAMES, name) -
      identifiers::SHIP_LOOK_ITEM_NAMES.begin());
}

Purse purseWithAnEmptyLocker(std::uint32_t kopecks) {
  Purse purse{.kopecks = kopecks, .locker = {}};
  purse.locker.items.reset();
  return purse;
}

}  // namespace rtype::client::testing
