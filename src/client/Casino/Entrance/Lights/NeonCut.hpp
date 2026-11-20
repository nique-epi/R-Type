#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A wait of the neon script of the pylon, and the cuts that open
 * it. */
struct NeonCut {
  engine::Duration wait{};
  int cuts{};
};

}  // namespace rtype::client
