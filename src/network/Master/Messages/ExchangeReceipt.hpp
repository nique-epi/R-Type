#pragma once

#include "Wallet.hpp"
#include "WalletExchange.hpp"

namespace rtype::network {

/** @brief An exchange just made, and the balances it left: the answer of
 * `POST /api/users/me/wallet/exchanges`. */
struct ExchangeReceipt {
  WalletExchange exchange;
  Wallet wallet;

  bool operator==(const ExchangeReceipt&) const = default;
};

}  // namespace rtype::network
