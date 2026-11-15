#pragma once

#include "IInterfaceCanvas.hpp"
#include "TicketView.hpp"

namespace rtype::client {

/**
 * @brief Paints the sales slip of a ware over the dimmed shop, as @p view
 * shows it: the paper of SHOP_TICKET_SHEET_ID in TICKET_AREA, its printed
 * form, the ware centred over its name and description, its price, then its
 * CANCEL and BUY buttons and its stub; or, once paid, its PAID stamp where
 * the buttons were and its stub torn off.
 */
void paintTicket(IInterfaceCanvas& canvas, const TicketView& view);

}  // namespace rtype::client
