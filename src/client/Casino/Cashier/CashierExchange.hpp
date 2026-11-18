#pragma once

#include <optional>
#include "Wallet.hpp"
#include "WalletExchangeRefusal.hpp"

namespace rtype::client {

/** @brief What became of an exchange tried at the cashier: why the rules of
 * the master refuse it (WalletBalances.hpp), or nothing once the amount is
 * changed, and the wallet after it, unchanged when refused. */
struct CashierExchange {
  std::optional<network::WalletExchangeRefusal> refusal;
  network::Wallet wallet;
};

}  // namespace rtype::client
