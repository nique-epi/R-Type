#include "CashierRules.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "CashierAttempt.hpp"
#include "CashierConstants.hpp"
#include "CashierExchange.hpp"
#include "CashierWay.hpp"
#include "CasinoPaintingConstants.hpp"
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "Kopecks.hpp"
#include "Language.hpp"
#include "TextMarks.hpp"
#include "Translation.hpp"
#include "Wallet.hpp"
#include "WalletBalances.hpp"
#include "WalletExchangeRefusal.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

namespace {

/** @returns The name of the balance @p way takes from. */
Translation sourceWordOf(CashierWay way) {
  return way == CashierWay::BuyChips ? CASINO_KOPECKS_CAPTION
                                     : CASINO_CHIPS_CAPTION;
}

/** @returns The name of the balance @p way gives to. */
Translation destinationWordOf(CashierWay way) {
  return way == CashierWay::BuyChips ? CASINO_CHIPS_CAPTION
                                     : CASINO_KOPECKS_CAPTION;
}

/** @returns @p line in @p language, the name of @p currency in its mark. */
std::string withCurrency(Translation line, Translation currency,
                         Language language) {
  return withMark(line.in(language), {.mark = CASHIER_CURRENCY_MARK,
                                      .value = currency.in(language)});
}

/** @returns Why the master did not make an exchange, in both languages. */
Translation refusalTextOf(WalletRefusal refusal) {
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

std::string cashierLineOf(const CashierAttempt& attempt, Language language) {
  if (!attempt.tried.refusal.has_value()) {
    return {};
  }
  switch (*attempt.tried.refusal) {
    case network::WalletExchangeRefusal::InvalidAmount:
      return std::string(NOTHING_TO_EXCHANGE_LINE.in(language));
    case network::WalletExchangeRefusal::NotEnough:
      return withMark(
          withCurrency(NOT_ENOUGH_LINE, sourceWordOf(attempt.way), language),
          {.mark = CASHIER_MISSING_MARK,
           .value = kopecksText(attempt.amount -
                                sourceOf(attempt.tried.wallet, attempt.way))});
    case network::WalletExchangeRefusal::PastTheMaximum:
      break;
  }
  return withCurrency(FULL_LINE, destinationWordOf(attempt.way), language);
}

std::string cashierRefusalLineOf(WalletRefusal refusal, Language language) {
  return std::string(refusalTextOf(refusal).in(language));
}

}  // namespace rtype::client
