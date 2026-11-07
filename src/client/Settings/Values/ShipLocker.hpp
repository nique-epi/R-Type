#pragma once

#include <bitset>
#include "ShipLockerConstants.hpp"
#include "ShipLookConstants.hpp"

namespace rtype::client {

/** @brief The items the player holds in the locker of the hangar, one bit per
 * item, in the order of identifiers::SHIP_LOOK_ITEM_NAMES. A first launch
 * holds the starter kit. */
struct ShipLocker {
  std::bitset<identifiers::SHIP_LOOK_ITEM_COUNT> items{STARTER_LOCKER_ITEMS};

  bool operator==(const ShipLocker& other) const = default;
};

}  // namespace rtype::client
