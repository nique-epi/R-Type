#pragma once

#include <cstdint>
#include "Currency.hpp"

namespace rtype::network {

/** @brief What a player asks the cashier: the body of
 * `POST /api/users/me/wallet/exchanges`. The amount leaves @p from and comes
 * into the other currency, one for one. */
struct ExchangeRequest {
  Currency from{Currency::Kopecks};
  std::uint32_t amount{};

  bool operator==(const ExchangeRequest&) const = default;
};

}  // namespace rtype::network
