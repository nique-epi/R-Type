#pragma once

#include "LookPurchaseOutcome.hpp"
#include "Wallet.hpp"

namespace rtype::master {

/** @brief What a purchase of a look did, and the balances it left. */
struct LookPurchaseResult {
  LookPurchaseOutcome outcome{LookPurchaseOutcome::Bought};
  network::Wallet wallet;
};

}  // namespace rtype::master
