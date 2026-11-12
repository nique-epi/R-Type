#pragma once

#include "IInterfaceCanvas.hpp"
#include "ShopView.hpp"
#include "ShopWares.hpp"

namespace rtype::client {

/** @brief Paints the shop: the picture of the booth, then over it the looks
 * of the rows shown and the hulls of the glass case, each with its price tag,
 * or a red ribbon and no tag once the player owns it, the cyan frame of the
 * chosen ware, the scroll mark of the locker, the balance and the status
 * line. */
void paintShop(IInterfaceCanvas& canvas, const ShopWares& wares,
               const ShopView& view);

}  // namespace rtype::client
