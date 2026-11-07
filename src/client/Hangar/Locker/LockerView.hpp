#pragma once

#include <cstddef>
#include "ShipLocker.hpp"
#include "ShipLook.hpp"

namespace rtype::client {

/** @brief What the locker shows: the chosen item, what the ship wears, and
 * the items held: a cell of an item not held stays empty. */
struct LockerView {
  std::size_t chosen{};
  identifiers::ShipLook worn;
  ShipLocker locker;
};

}  // namespace rtype::client
