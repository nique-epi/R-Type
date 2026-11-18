#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include "CashierExchange.hpp"
#include "CashierWay.hpp"
#include "CasinoBalances.hpp"

namespace rtype::client {

/** @brief What the cashier shows under its bars: its way, the wallet of the
 * player as the exchange tried reads it, the amount and the step chosen, and
 * the exchange that amount would make. */
struct CashierView {
  CashierWay way{};
  std::uint32_t amount{};
  /** @brief The place of the step in CHIP_VALUES. */
  std::size_t step{};
  /** @brief What changing the amount would give, or why it is refused. */
  CashierExchange tried;
  /** @brief The balances shown top right, nothing for one the master has
   * not answered. */
  CasinoBalances balances;
  /** @brief What the cashier says in place of the result: the master asked
   * or refusing; nothing otherwise. */
  std::optional<std::string> notice;
};

}  // namespace rtype::client
