#pragma once

#include <cstddef>
#include "ShipLook.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A fitting under way: the item an arm brings, when it started, on
 * the clock of the screen, and what the ship wore before. */
struct FittingRun {
  std::size_t item{};
  engine::Duration started{};
  ShipLook before;
};

}  // namespace rtype::client
