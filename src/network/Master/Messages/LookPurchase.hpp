#pragma once

#include <string>
#include "Wallet.hpp"

namespace rtype::network {

/** @brief A look the account owns after `PUT /api/users/me/looks/{item}`,
 * and the balances left: the answer of that route, whether it bought the look
 * or the account already owned it. */
struct LookPurchase {
  /** @brief Its name in identifiers::SHIP_LOOK_ITEM_NAMES. */
  std::string item;
  Wallet wallet;

  bool operator==(const LookPurchase&) const = default;
};

}  // namespace rtype::network
