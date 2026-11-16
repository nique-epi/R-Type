#pragma once

#include <chrono>
#include <cstdint>
#include "Wallet.hpp"

namespace rtype::master {

/** @brief The wallet of an account as it opens, at the first read of it. */
struct WalletOpening {
  std::int64_t userId{};
  /** @brief The balances it starts with, from the settings
   * `wallet-starting-kopecks` and `wallet-starting-chips`. */
  network::Wallet starting;
  std::chrono::sys_seconds openedAt;
};

}  // namespace rtype::master
