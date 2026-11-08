#pragma once

#include <cstddef>
#include "ShipLook.hpp"

namespace rtype::server {

/** @brief What LookRelay keeps of the player on one seat: its look, and the
 * first tick at which it may choose again. */
struct LookSeat {
  identifiers::ShipLook look;
  std::size_t nextChoiceTick{0};
};

}  // namespace rtype::server
