#include "FakeWallet.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "ExchangeRequest.hpp"
#include "ShipLocker.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"

namespace {

/** @returns @p answer when it answers @p ask, nothing otherwise. */
std::optional<rtype::client::WalletChangeReading> answerTo(
    const std::optional<rtype::client::WalletChangeReading>& answer,
    std::uint64_t ask) {
  if (!answer.has_value() || answer->ask != ask) {
    return std::nullopt;
  }
  return answer;
}

}  // namespace

std::optional<rtype::network::Wallet> FakeWallet::wallet() const {
  return balances;
}

std::optional<rtype::client::ShipLocker> FakeWallet::ownedLooks() const {
  return looks;
}

void FakeWallet::askWallet() { ++walletAsks; }

std::uint64_t FakeWallet::buy(std::size_t item) {
  boughtItems.push_back(item);
  return boughtItems.size();
}

std::optional<rtype::client::WalletChangeReading> FakeWallet::purchase(
    std::uint64_t ask) const {
  return answerTo(purchaseAnswer, ask);
}

std::uint64_t FakeWallet::exchange(
    const rtype::network::ExchangeRequest& request) {
  exchangesAsked.push_back(request);
  return exchangesAsked.size();
}

std::optional<rtype::client::WalletChangeReading> FakeWallet::exchanged(
    std::uint64_t ask) const {
  return answerTo(exchangeAnswer, ask);
}
