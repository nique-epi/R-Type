#pragma once

#include <cstdint>
#include <string>
#include "ExchangeRequest.hpp"

namespace rtype::client {

/**
 * @brief What the frame loop asked of the account so far, as MasterAccountLink
 * hands it to its thread: each ask has a number one above the previous one of
 * its kind, so the thread serves the newest ask only, and a reading tells
 * which ask it answers. 0 means not asked yet.
 *
 * The tokens are kept in memory only: never logged, never written.
 */
struct AccountAsks {
  std::uint64_t userAsk{};
  /** @brief The token whose account userAsk asks for. */
  std::string userToken;
  std::uint64_t walletAsk{};
  std::uint64_t purchaseAsk{};
  /** @brief The name of the look purchaseAsk buys. */
  std::string purchaseItem;
  std::uint64_t exchangeAsk{};
  /** @brief The exchange exchangeAsk asks of the cashier. */
  network::ExchangeRequest exchangeRequest;
  /** @brief The token the newest ask for the wallet, a purchase or an
   * exchange was made with. */
  std::string token;
};

}  // namespace rtype::client
