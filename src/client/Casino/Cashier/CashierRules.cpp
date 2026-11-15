#include "CashierRules.hpp"
#include <cstdint>
#include <string>
#include "CashierConstants.hpp"
#include "CashierWay.hpp"
#include "ChipsExchange.hpp"
#include "Exchange.hpp"
#include "ExchangeRefusal.hpp"
#include "Kopecks.hpp"
#include "Purse.hpp"

namespace rtype::client {

namespace {

/** @returns The name of the balance @p way takes from. */
std::string sourceWordOf(CashierWay way) {
  return way == CashierWay::BuyChips ? KOPECKS_WORD : CHIPS_WORD;
}

/** @returns The name of the balance @p way gives to. */
std::string destinationWordOf(CashierWay way) {
  return way == CashierWay::BuyChips ? CHIPS_WORD : KOPECKS_WORD;
}

}  // namespace

Exchange exchangeAt(const Purse& purse, CashierWay way, std::uint32_t amount) {
  return way == CashierWay::BuyChips ? buyChips(purse, amount)
                                     : cashOutChips(purse, amount);
}

std::uint32_t sourceOf(const Purse& purse, CashierWay way) {
  return way == CashierWay::BuyChips ? purse.kopecks : purse.chips;
}

std::uint32_t destinationOf(const Purse& purse, CashierWay way) {
  return way == CashierWay::BuyChips ? purse.chips : purse.kopecks;
}

std::string cashierLineOf(const Exchange& tried, CashierWay way,
                          std::uint32_t amount) {
  if (!tried.refusal.has_value()) {
    return {};
  }
  switch (*tried.refusal) {
    case ExchangeRefusal::NothingToExchange:
      return NOTHING_TO_EXCHANGE_LINE;
    case ExchangeRefusal::NotEnoughKopecks:
    case ExchangeRefusal::NotEnoughChips:
      return NOT_ENOUGH_START + sourceWordOf(way) + MISSING_SEPARATOR +
             kopecksText(amount - sourceOf(tried.purse, way)) + MISSING_END;
    case ExchangeRefusal::BalanceFull:
      break;
  }
  return FULL_START + destinationWordOf(way);
}

}  // namespace rtype::client
