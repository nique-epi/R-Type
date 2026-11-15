#pragma once

#include "CashierView.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief Paints @p view, everything the cashier shows but its bars: the
 * hall dimmed under it, the balances top right, then its gold panel: the
 * balance it takes from and the one it gives to, the amount between its
 * marks and the step, the chips of the selector with their keys, the chosen
 * one raised and in corners, then what both balances hold after the
 * exchange, or why it is refused.
 */
void paintCashier(IInterfaceCanvas& canvas, const CashierView& view);

}  // namespace rtype::client
