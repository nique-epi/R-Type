#pragma once

#include "Endpoint.hpp"
#include "TwoWayTraffic.hpp"

namespace rtype::network {

/** @brief The traffic exchanged with one followed peer. */
struct PeerTraffic {
  Endpoint peer;
  TwoWayTraffic traffic;
};

}  // namespace rtype::network
