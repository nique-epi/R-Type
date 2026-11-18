#include "CashierRules.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "CashierConstants.hpp"
#include "CashierExchange.hpp"
#include "CashierWay.hpp"
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "Kopecks.hpp"
#include "Wallet.hpp"
#include "WalletBalances.hpp"
#include "WalletExchangeRefusal.hpp"
#include "WalletRefusal.hpp"

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

/** @returns The currency @p way takes from: the kopecks to buy chips, the
 * chips to cash out. */
network::Currency currencyTakenBy(CashierWay way) {
  return way == CashierWay::BuyChips ? network::Currency::Kopecks
                                     : network::Currency::Chips;
}

}  // namespace

network::ExchangeRequest exchangeRequestOf(CashierWay way,
                                           std::uint32_t amount) {
  return network::ExchangeRequest{.from = currencyTakenBy(way),
                                  .amount = amount};
}

CashierExchange exchangeAt(const network::Wallet& wallet, CashierWay way,
                           std::uint32_t amount) {
  const network::ExchangeRequest request = exchangeRequestOf(way, amount);
  if (const std::optional<network::WalletExchangeRefusal> refusal =
          network::exchangeRefusalOf(wallet, request)) {
    return CashierExchange{.refusal = refusal, .wallet = wallet};
  }
  return CashierExchange{.refusal = std::nullopt,
                         .wallet = network::exchangedWallet(wallet, request)};
}

std::uint32_t sourceOf(const network::Wallet& wallet, CashierWay way) {
  return network::balanceIn(wallet, currencyTakenBy(way));
}

std::uint32_t destinationOf(const network::Wallet& wallet, CashierWay way) {
  return network::balanceIn(wallet,
                            network::otherCurrency(currencyTakenBy(way)));
}

std::string cashierLineOf(const CashierExchange& tried, CashierWay way,
                          std::uint32_t amount) {
  if (!tried.refusal.has_value()) {
    return {};
  }
  switch (*tried.refusal) {
    case network::WalletExchangeRefusal::InvalidAmount:
      return NOTHING_TO_EXCHANGE_LINE;
    case network::WalletExchangeRefusal::NotEnough:
      return NOT_ENOUGH_START + sourceWordOf(way) + MISSING_SEPARATOR +
             kopecksText(amount - sourceOf(tried.wallet, way)) + MISSING_END;
    case network::WalletExchangeRefusal::PastTheMaximum:
      break;
  }
  return FULL_START + destinationWordOf(way);
}

std::string cashierRefusalLineOf(WalletRefusal refusal) {
  switch (refusal) {
    case WalletRefusal::BalanceRefused:
      return BALANCES_REFUSED_LINE;
    case WalletRefusal::SignedOut:
      return SIGN_IN_AGAIN_LINE;
    case WalletRefusal::MasterUnreachable:
      return MASTER_SILENT_LINE;
    case WalletRefusal::InvalidAmount:
      return NOTHING_TO_EXCHANGE_LINE;
    case WalletRefusal::NotForSale:
    case WalletRefusal::UnknownItem:
    case WalletRefusal::MasterRefused:
      return MASTER_REFUSED_LINE;
  }
  return MASTER_REFUSED_LINE;
}

}  // namespace rtype::client
