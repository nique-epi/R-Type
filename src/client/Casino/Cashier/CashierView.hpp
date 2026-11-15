#pragma once

#include <cstddef>
#include <cstdint>
#include "CashierWay.hpp"
#include "Exchange.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @brief What the cashier shows under its bars: its way, the purse of the
 * player, the amount and the step chosen, and the exchange that amount
 * would make. */
struct CashierView {
  CashierWay way{};
  Purse purse;
  std::uint32_t amount{};
  /** @brief The place of the step in CHIP_VALUES. */
  std::size_t step{};
  /** @brief What changing the amount would give, or why it is refused. */
  Exchange tried;
};

}  // namespace rtype::client
