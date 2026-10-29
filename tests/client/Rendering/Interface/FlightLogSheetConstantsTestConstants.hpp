#pragma once

#include <array>
#include "FlightLogSheetConstants.hpp"

namespace rtype::client::testing {

/** @brief Every frame of the flight log sheet. */
constexpr std::array FLIGHT_LOG_FRAMES{
    rtype::client::COVER_FRONT_FRAME, rtype::client::COVER_INSIDE_FRAME,
    rtype::client::RIGHT_HALF_FRAME,  rtype::client::TAUNT_LABEL_FRAME,
    rtype::client::SHADOW_TILE_FRAME, rtype::client::MEDAL_FRAME,
    rtype::client::CROSS_FRAME};

}  // namespace rtype::client::testing
