#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Why the cashier of the casino does not change an amount. */
enum class ExchangeRefusal : std::uint8_t {
  NothingToExchange,
  NotEnoughKopecks,
  NotEnoughChips,
  BalanceFull,
};

}  // namespace rtype::client
