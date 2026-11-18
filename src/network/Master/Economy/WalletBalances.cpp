#include "WalletBalances.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "KopecksConstants.hpp"
#include "Wallet.hpp"
#include "WalletExchangeRefusal.hpp"

namespace rtype::network {

std::uint32_t balanceIn(const Wallet& wallet, Currency currency) {
  return currency == Currency::Kopecks ? wallet.kopecks : wallet.chips;
}

Wallet withBalance(Wallet wallet, Currency currency, std::uint32_t balance) {
  if (currency == Currency::Kopecks) {
    wallet.kopecks = balance;
  } else {
    wallet.chips = balance;
  }
  return wallet;
}

std::uint32_t maximumOf(Currency currency) {
  return currency == Currency::Kopecks ? MAXIMUM_KOPECKS : MAXIMUM_CHIPS;
}

Currency otherCurrency(Currency currency) {
  return currency == Currency::Kopecks ? Currency::Chips : Currency::Kopecks;
}

bool canSpend(const Wallet& wallet, Currency currency, std::uint32_t amount) {
  return amount <= balanceIn(wallet, currency);
}

bool canReceive(const Wallet& wallet, Currency currency, std::uint32_t amount) {
  return std::uint64_t{balanceIn(wallet, currency)} + amount <=
         maximumOf(currency);
}

std::uint32_t creditedBalance(const Wallet& wallet, Currency currency,
                              std::uint64_t gain) {
  const std::uint64_t room =
      maximumOf(currency) - std::uint64_t{balanceIn(wallet, currency)};
  return balanceIn(wallet, currency) +
         static_cast<std::uint32_t>(std::min(gain, room));
}

std::optional<WalletExchangeRefusal> exchangeRefusalOf(
    const Wallet& before, const ExchangeRequest& request) {
  if (request.amount == 0 || request.amount > maximumOf(request.from)) {
    return WalletExchangeRefusal::InvalidAmount;
  }
  if (!canSpend(before, request.from, request.amount)) {
    return WalletExchangeRefusal::NotEnough;
  }
  if (!canReceive(before, otherCurrency(request.from), request.amount)) {
    return WalletExchangeRefusal::PastTheMaximum;
  }
  return std::nullopt;
}

Wallet exchangedWallet(const Wallet& before, const ExchangeRequest& request) {
  const Currency into = otherCurrency(request.from);
  const Wallet spent = withBalance(
      before, request.from, balanceIn(before, request.from) - request.amount);
  return withBalance(spent, into, balanceIn(spent, into) + request.amount);
}

}  // namespace rtype::network
