#pragma once

#include <string>
#include "Endpoint.hpp"

namespace rtype::server {

/** @brief A player that asks the supervisor for a place, with a valid
 * cookie: where it sends from and the nickname it asks for. */
struct SeatRequest {
  network::Endpoint sender;
  std::string nickname;
};

}  // namespace rtype::server
