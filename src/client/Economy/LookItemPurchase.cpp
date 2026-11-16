#include "LookItemPurchase.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "KopecksConstants.hpp"
#include "LookItems.hpp"
#include "Purchase.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"

namespace rtype::client {

namespace {

/** @returns Why @p purse cannot buy @p item, the first that holds; nothing
 * when it can. */
std::optional<PurchaseRefusal> purchaseRefusalOf(const Purse& purse,
                                                 std::size_t item) {
  if (purse.locker.items.test(item)) {
    return PurchaseRefusal::AlreadyOwned;
  }
  const std::uint32_t price = network::lookItemPrice(item);
  if (price == network::NOT_FOR_SALE) {
    return PurchaseRefusal::NotForSale;
  }
  if (price > purse.kopecks) {
    return PurchaseRefusal::TooExpensive;
  }
  return std::nullopt;
}

}  // namespace

Purchase buyLookItem(const Purse& purse, std::size_t item) {
  if (const std::optional<PurchaseRefusal> refusal =
          purchaseRefusalOf(purse, item)) {
    return Purchase{.refusal = refusal, .purse = purse};
  }
  Purse after = purse;
  after.kopecks -= network::lookItemPrice(item);
  after.locker.items.set(item);
  return Purchase{.refusal = std::nullopt, .purse = after};
}

}  // namespace rtype::client
