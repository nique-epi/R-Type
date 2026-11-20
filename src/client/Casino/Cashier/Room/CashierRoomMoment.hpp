#pragma once

#include <optional>
#include "CashierWay.hpp"
#include "MotionPreference.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief When the cage of the cashier is painted: how long it has been
 * open, how long ago the teller started serving the last exchange, if he
 * still serves it, the way the panel changes, which the rate board lights,
 * and whether the player asked for less motion, which keeps it still. */
struct CashierRoomMoment {
  engine::Duration since{};
  std::optional<engine::Duration> sinceServed;
  CashierWay way{};
  MotionPreference motion{};
};

}  // namespace rtype::client
