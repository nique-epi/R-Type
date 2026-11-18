#pragma once

#include <cstdint>
#include <optional>
#include "Wallet.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

/** @brief What the master answered to a purchase or an exchange: the
 * balances it left once made, or why it was refused. */
struct WalletChangeReading {
  /** @brief The number of the ask it answers (see AccountAsks). */
  std::uint64_t ask{};
  std::optional<network::Wallet> wallet;
  std::optional<WalletRefusal> refusal;
};

}  // namespace rtype::client
