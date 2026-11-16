#pragma once

#include <vector>
#include "WalletMovement.hpp"

namespace rtype::network {

/** @brief One page of the journal of a wallet, newest first: the answer of
 * `GET /api/users/me/wallet/movements`. */
struct MovementListing {
  /** @brief MAXIMUM_PAGE_SIZE at most. */
  std::vector<WalletMovement> movements;

  bool operator==(const MovementListing&) const = default;
};

}  // namespace rtype::network
