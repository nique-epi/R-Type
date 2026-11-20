#pragma once

#include "MotionPreference.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief When the hall is painted: how long it has been open, which sets
 * where every light and every spectator is, and whether the player asked
 * for less motion, which keeps it still. */
struct HallMoment {
  engine::Duration since{};
  MotionPreference motion{};
};

}  // namespace rtype::client
