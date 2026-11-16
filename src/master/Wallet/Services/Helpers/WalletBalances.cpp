#include "WalletBalances.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "Currency.hpp"
#include "KopecksConstants.hpp"
#include "LookItems.hpp"
#include "ShipLookConstants.hpp"
#include "Wallet.hpp"

namespace rtype::master {

std::uint32_t balanceIn(const network::Wallet& wallet,
                        network::Currency currency) {
  return currency == network::Currency::Kopecks ? wallet.kopecks : wallet.chips;
}

network::Wallet withBalance(network::Wallet wallet, network::Currency currency,
                            std::uint32_t balance) {
  if (currency == network::Currency::Kopecks) {
    wallet.kopecks = balance;
  } else {
    wallet.chips = balance;
  }
  return wallet;
}

std::uint32_t maximumOf(network::Currency currency) {
  return currency == network::Currency::Kopecks ? network::MAXIMUM_KOPECKS
                                                : network::MAXIMUM_CHIPS;
}

network::Currency otherCurrency(network::Currency currency) {
  return currency == network::Currency::Kopecks ? network::Currency::Chips
                                                : network::Currency::Kopecks;
}

bool canSpend(const network::Wallet& wallet, network::Currency currency,
              std::uint32_t amount) {
  return amount <= balanceIn(wallet, currency);
}

bool canReceive(const network::Wallet& wallet, network::Currency currency,
                std::uint32_t amount) {
  return std::uint64_t{balanceIn(wallet, currency)} + amount <=
         maximumOf(currency);
}

std::uint32_t creditedBalance(const network::Wallet& wallet,
                              network::Currency currency, std::uint64_t gain) {
  const std::uint64_t room =
      maximumOf(currency) - std::uint64_t{balanceIn(wallet, currency)};
  return balanceIn(wallet, currency) +
         static_cast<std::uint32_t>(std::min(gain, room));
}

std::vector<std::string> inCatalogueOrder(std::vector<std::string> items) {
  std::ranges::stable_sort(items, {}, [](const std::string& item) {
    return network::lookItemOfName(item).value_or(
        identifiers::SHIP_LOOK_ITEM_COUNT);
  });
  return items;
}

}  // namespace rtype::master
