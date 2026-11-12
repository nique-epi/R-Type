#pragma once

#include <cstdint>
#include <string_view>
#include "RowWindow.hpp"
#include "ShipLocker.hpp"
#include "ShopSpot.hpp"
#include "ShopkeeperPose.hpp"

namespace rtype::client {

/** @brief What the shop shows over its picture: the chosen ware, the rows of
 * the locker shown, what the player owns and holds, its status line, its
 * shopkeeper and its radio. */
struct ShopView {
  ShopSpot chosen;
  RowWindow rows;
  ShipLocker owned;
  std::uint32_t kopecks{};
  std::string_view status;
  /** @brief How the shopkeeper stands now. */
  ShopkeeperPose keeper;
  /** @brief Whether the pointer is over the radio, which then shows the
   * frame of a choice. */
  bool radioPointed{false};
};

}  // namespace rtype::client
