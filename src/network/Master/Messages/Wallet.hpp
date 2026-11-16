#pragma once

#include <cstdint>

namespace rtype::network {

/** @brief The balances of the wallet of an account: the answer of
 * `GET /api/users/me/wallet`. Each is 0 to MAXIMUM_KOPECKS or
 * MAXIMUM_CHIPS. */
struct Wallet {
  std::uint32_t kopecks{};
  std::uint32_t chips{};

  bool operator==(const Wallet&) const = default;
};

}  // namespace rtype::network
