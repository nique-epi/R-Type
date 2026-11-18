#pragma once

#include <cstdint>
#include "Wallet.hpp"

namespace rtype::master {

/** @brief The wallet a game credit is written on: the account its ticket was
 * given to, and the balances it held before. */
struct CreditedWallet {
  std::int64_t userId{};
  network::Wallet before;
};

}  // namespace rtype::master
