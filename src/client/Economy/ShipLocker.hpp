#pragma once

#include <bitset>
#include "ShipLookConstants.hpp"
#include "StarterLookConstants.hpp"

namespace rtype::client {

/** @brief The items the player holds in the locker of the hangar, one bit per
 * item, in the order of identifiers::SHIP_LOOK_ITEM_NAMES: the looks an
 * account owns on the master. A default-built locker holds the starter kit,
 * all a guest wears. */
struct ShipLocker {
  std::bitset<identifiers::SHIP_LOOK_ITEM_COUNT> items{
      network::STARTER_LOCKER_ITEMS};

  bool operator==(const ShipLocker& other) const = default;
};

}  // namespace rtype::client
