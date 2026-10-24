#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The shape of a shockwave: its last radius, in pixels of the
 * canvas, and its life. */
struct Ring {
  int radius{};
  engine::Duration life{};
};

}  // namespace rtype::client
