#pragma once

#include <cstdint>
#include <string_view>
#include "RowWindow.hpp"
#include "ShipLocker.hpp"
#include "ShopSpot.hpp"

namespace rtype::client {

/** @brief What the shop shows over its picture: the chosen ware, the rows of
 * the locker shown, what the player owns and holds, and its status line. */
struct ShopView {
  ShopSpot chosen;
  RowWindow rows;
  ShipLocker owned;
  std::uint32_t kopecks{};
  std::string_view status;
};

}  // namespace rtype::client
