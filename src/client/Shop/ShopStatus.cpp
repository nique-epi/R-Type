#include "ShopStatus.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include "LookItemPurchase.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"
#include "ShipLooks.hpp"
#include "ShopScreenConstants.hpp"

namespace rtype::client {

std::string wareLineOf(std::size_t item, const Purse& purse) {
  std::string line(lookItemLabelOf(item));
  if (purse.locker.items.test(item)) {
    return line.append(OWNED_HINT);
  }
  return line.append(BUY_HINT_START)
      .append(std::to_string(lookItemPrice(item)))
      .append(BUY_HINT_END);
}

std::string boughtLineOf(std::size_t item) {
  return std::string(BOUGHT_STATUS_START).append(lookItemLabelOf(item));
}

std::string refusalLineOf(PurchaseRefusal refusal, std::uint32_t missing) {
  switch (refusal) {
    case PurchaseRefusal::AlreadyOwned:
      return std::string(ALREADY_YOURS_STATUS);
    case PurchaseRefusal::NotForSale:
      return std::string(NOT_FOR_SALE_STATUS);
    case PurchaseRefusal::TooExpensive:
      break;
  }
  return std::string(NOT_ENOUGH_STATUS_START)
      .append(std::to_string(missing))
      .append(NOT_ENOUGH_STATUS_END);
}

}  // namespace rtype::client
