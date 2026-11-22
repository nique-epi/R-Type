#include "ShopStatus.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "Kopecks.hpp"
#include "Language.hpp"
#include "LookItems.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"
#include "ShipLooks.hpp"
#include "ShopScreenConstants.hpp"
#include "TextMark.hpp"
#include "TextMarks.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

std::string wareLineOf(std::size_t item, const Purse& purse,
                       Language language) {
  const std::string_view name = lookItemLabelOf(item, language);
  if (purse.locker.items.test(item)) {
    return withMark(std::string(OWNED_HINT.in(language)),
                    TextMark{.mark = ITEM_MARK, .value = name});
  }
  const std::string price = kopecksText(network::lookItemPrice(item));
  return withMark(withMark(std::string(BUY_HINT.in(language)),
                           TextMark{.mark = ITEM_MARK, .value = name}),
                  TextMark{.mark = PRICE_MARK, .value = price});
}

std::string boughtLineOf(std::size_t item, Language language) {
  return withMark(
      std::string(BOUGHT_STATUS.in(language)),
      TextMark{.mark = ITEM_MARK, .value = lookItemLabelOf(item, language)});
}

std::string refusalLineOf(PurchaseRefusal refusal, std::uint32_t missing,
                          Language language) {
  switch (refusal) {
    case PurchaseRefusal::AlreadyOwned:
      return std::string(ALREADY_YOURS_STATUS.in(language));
    case PurchaseRefusal::NotForSale:
      return std::string(NOT_FOR_SALE_STATUS.in(language));
    case PurchaseRefusal::TooExpensive:
      break;
  }
  const std::string short_ = kopecksText(missing);
  return withMark(std::string(NOT_ENOUGH_STATUS.in(language)),
                  TextMark{.mark = MISSING_MARK, .value = short_});
}

std::string masterRefusalLineOf(WalletRefusal refusal, Language language) {
  switch (refusal) {
    case WalletRefusal::BalanceRefused:
      return std::string(NOT_ENOUGH_KOPECKS_STATUS.in(language));
    case WalletRefusal::NotForSale:
    case WalletRefusal::UnknownItem:
      return std::string(NOT_FOR_SALE_STATUS.in(language));
    case WalletRefusal::SignedOut:
      return std::string(SESSION_ENDED_STATUS.in(language));
    case WalletRefusal::InvalidAmount:
    case WalletRefusal::MasterRefused:
      return std::string(MASTER_REFUSED_STATUS.in(language));
    case WalletRefusal::MasterUnreachable:
      return std::string(MASTER_SILENT_STATUS.in(language));
  }
  return std::string(MASTER_REFUSED_STATUS.in(language));
}

std::optional<PurchaseRefusal> purchaseRefusalOf(WalletRefusal refusal) {
  switch (refusal) {
    case WalletRefusal::BalanceRefused:
      return PurchaseRefusal::TooExpensive;
    case WalletRefusal::NotForSale:
    case WalletRefusal::UnknownItem:
      return PurchaseRefusal::NotForSale;
    case WalletRefusal::InvalidAmount:
    case WalletRefusal::SignedOut:
    case WalletRefusal::MasterRefused:
    case WalletRefusal::MasterUnreachable:
      return std::nullopt;
  }
  return std::nullopt;
}

}  // namespace rtype::client
