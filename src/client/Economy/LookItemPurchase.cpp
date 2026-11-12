#include "LookItemPurchase.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "EconomyConstants.hpp"
#include "Purchase.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"
#include "ShipLookPriceConstants.hpp"

namespace rtype::client {

std::uint32_t lookItemPrice(std::size_t item) {
  return SHIP_LOOK_ITEM_PRICES.at(item);
}

std::optional<PurchaseRefusal> purchaseRefusalOf(const Purse& purse,
                                                 std::size_t item) {
  if (purse.locker.items.test(item)) {
    return PurchaseRefusal::AlreadyOwned;
  }
  const std::uint32_t price = lookItemPrice(item);
  if (price == NOT_FOR_SALE) {
    return PurchaseRefusal::NotForSale;
  }
  if (price > purse.kopecks) {
    return PurchaseRefusal::TooExpensive;
  }
  return std::nullopt;
}

Purchase buyLookItem(const Purse& purse, std::size_t item) {
  if (const std::optional<PurchaseRefusal> refusal =
          purchaseRefusalOf(purse, item)) {
    return Purchase{.refusal = refusal, .purse = purse};
  }
  Purse after = purse;
  after.kopecks -= lookItemPrice(item);
  after.locker.items.set(item);
  return Purchase{.refusal = std::nullopt, .purse = after};
}

}  // namespace rtype::client
