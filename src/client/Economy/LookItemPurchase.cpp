#include "LookItemPurchase.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "KopecksConstants.hpp"
#include "LookItems.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"

namespace rtype::client {

std::optional<PurchaseRefusal> lookRefusalOf(const Purse& purse,
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

}  // namespace rtype::client
