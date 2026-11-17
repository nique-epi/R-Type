#pragma once

#include <cstddef>
#include "TicketButton.hpp"

namespace rtype::client {

/** @brief What the sales slip shows: the ware, a position in
 * SHIP_LOOK_ITEM_NAMES, the button chosen, and whether it was paid, its
 * stamp on it and its stub torn off. */
struct TicketView {
  std::size_t item{};
  TicketButton chosen{TicketButton::Buy};
  bool paid{false};
};

}  // namespace rtype::client
