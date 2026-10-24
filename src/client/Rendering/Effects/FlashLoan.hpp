#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Pixels an effect flashes, and for how long from now. */
struct FlashLoan {
  int area{};
  engine::Duration remaining{};
};

}  // namespace rtype::client
