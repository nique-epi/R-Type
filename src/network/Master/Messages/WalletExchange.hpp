#pragma once

#include <chrono>
#include <cstdint>
#include "Currency.hpp"

namespace rtype::network {

/** @brief An exchange the cashier made: the answer of
 * `GET /api/users/me/wallet/exchanges/{id}`. */
struct WalletExchange {
  /** @brief The number of the line of the journal that took the amount. */
  std::int64_t id{};
  /** @brief The currency the amount left; it came into the other one. */
  Currency from{Currency::Kopecks};
  std::uint32_t amount{};
  std::chrono::sys_seconds madeAt;

  bool operator==(const WalletExchange&) const = default;
};

}  // namespace rtype::network
