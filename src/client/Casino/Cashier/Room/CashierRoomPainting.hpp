#pragma once

#include <optional>
#include "CashierRoom.hpp"
#include "CashierRoomMoment.hpp"
#include "IInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns Whether the teller still serves @p sinceServed after he started
 * serving an exchange: the eight frames of his serving have not all shown
 * yet; never when he serves none. */
[[nodiscard]] bool tellerServesAt(std::optional<engine::Duration> sinceServed);

/**
 * @brief Paints the cage of the cashier, everything but its panel: the
 * picture, the rate board lit for the way of @p moment, the bills that
 * shift, the glint on the chip rack, the light of the camera and the coins
 * that sparkle, then the teller, counting or serving, behind the counter and
 * the brass of his grille. With MotionPreference::Reduced, the cage stays
 * still and the teller on his first frame.
 */
void paintCashierRoom(IInterfaceCanvas& canvas, const CashierRoom& room,
                      const CashierRoomMoment& moment);

}  // namespace rtype::client
