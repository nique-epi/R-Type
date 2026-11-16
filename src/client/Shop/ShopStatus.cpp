#include "ShopStatus.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include "Kopecks.hpp"
#include "LookItems.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"
#include "ShipLooks.hpp"
#include "ShopScreenConstants.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

std::string wareLineOf(std::size_t item, const Purse& purse) {
  std::string line(lookItemLabelOf(item));
  if (purse.locker.items.test(item)) {
    return line.append(OWNED_HINT);
  }
  return line.append(BUY_HINT_START)
      .append(kopecksText(network::lookItemPrice(item)))
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
      .append(kopecksText(missing))
      .append(NOT_ENOUGH_STATUS_END);
}

std::string masterRefusalLineOf(WalletRefusal refusal) {
  switch (refusal) {
    case WalletRefusal::BalanceRefused:
      return std::string(NOT_ENOUGH_KOPECKS_STATUS);
    case WalletRefusal::NotForSale:
    case WalletRefusal::UnknownItem:
      return std::string(NOT_FOR_SALE_STATUS);
    case WalletRefusal::SignedOut:
      return std::string(SESSION_ENDED_STATUS);
    case WalletRefusal::MasterRefused:
      return std::string(MASTER_REFUSED_STATUS);
    case WalletRefusal::MasterUnreachable:
      return std::string(MASTER_SILENT_STATUS);
  }
  return std::string(MASTER_REFUSED_STATUS);
}

std::optional<PurchaseRefusal> purchaseRefusalOf(WalletRefusal refusal) {
  switch (refusal) {
    case WalletRefusal::BalanceRefused:
      return PurchaseRefusal::TooExpensive;
    case WalletRefusal::NotForSale:
    case WalletRefusal::UnknownItem:
      return PurchaseRefusal::NotForSale;
    case WalletRefusal::SignedOut:
    case WalletRefusal::MasterRefused:
    case WalletRefusal::MasterUnreachable:
      return std::nullopt;
  }
  return std::nullopt;
}

}  // namespace rtype::client
