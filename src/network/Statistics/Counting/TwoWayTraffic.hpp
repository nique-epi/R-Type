#pragma once

#include "DirectionTraffic.hpp"

namespace rtype::network {

/** @brief What crossed a socket both ways, for one row of a TrafficReport. */
struct TwoWayTraffic {
  DirectionTraffic sent;
  DirectionTraffic received;
};

}  // namespace rtype::network
