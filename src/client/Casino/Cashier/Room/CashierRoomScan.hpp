#pragma once

#include <SFML/Graphics/Image.hpp>
#include "CashierRoom.hpp"

namespace rtype::client {

/** @returns What the cashier reads in @p picture, the picture of its cage,
 * CASHIER_ROOM_ID: nothing when it is not as large as the interface. */
[[nodiscard]] CashierRoom cashierRoomOf(const sf::Image& picture);

}  // namespace rtype::client
