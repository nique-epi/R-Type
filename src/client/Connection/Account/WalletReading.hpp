#pragma once

#include <optional>
#include <string>
#include "OwnedLooks.hpp"
#include "Wallet.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for the wallet of a token: its
 * balances and the looks it owns, each nothing when the master did not answer
 * it, refused it, or answered what the client cannot read. */
struct WalletReading {
  /** @brief The token it was read with, so a reading of another session is
   * never shown; kept in memory only. */
  std::string token;
  std::optional<network::Wallet> wallet;
  std::optional<network::OwnedLooks> looks;
};

}  // namespace rtype::client
