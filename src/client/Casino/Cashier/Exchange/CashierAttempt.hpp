#pragma once

#include <cstdint>
#include "CashierExchange.hpp"
#include "CashierWay.hpp"

namespace rtype::client {

/** @brief An exchange the player tries at the cashier: what the rules of the
 * master make of it, its way, and the amount it changes. */
struct CashierAttempt {
  CashierExchange tried;
  CashierWay way{};
  std::uint32_t amount{};
};

}  // namespace rtype::client
