#pragma once

#include "CashierView.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief Paints the exchange panel of @p view, everything but its bars: its
 * tag, the balance it takes from and the one it gives to, the amount between
 * its marks and the step, the five chips, the chosen one framed, then what
 * both balances hold after the exchange, or, in red, why it is refused, or,
 * in gold, what the cashier does.
 */
void paintCashierPanel(IInterfaceCanvas& canvas, const CashierView& view);

}  // namespace rtype::client
