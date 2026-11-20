#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "CashierExchange.hpp"
#include "CashierNotice.hpp"
#include "CashierWay.hpp"

namespace rtype::client {

/** @brief What the panel of the cashier shows under its bars: its way, the
 * amount and the step chosen, the exchange that amount would make with the
 * wallet of the player, and what the cashier says in place of its result. */
struct CashierView {
  CashierWay way{};
  std::uint32_t amount{};
  /** @brief The place of the step in CHIP_VALUES. */
  std::size_t step{};
  /** @brief What changing the amount would give, or why it is refused. */
  CashierExchange tried;
  /** @brief What the cashier says in place of the result: the master asked,
   * serving or refusing; nothing otherwise. */
  std::optional<CashierNotice> notice;
};

}  // namespace rtype::client
