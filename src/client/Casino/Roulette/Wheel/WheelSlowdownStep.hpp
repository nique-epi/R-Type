#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A step of the slowing rotor: that many frames, each shown that
 * long. */
struct WheelSlowdownStep {
  engine::Duration frameTime{};
  int frames{};
};

}  // namespace rtype::client
