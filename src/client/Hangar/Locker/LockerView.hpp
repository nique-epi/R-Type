#pragma once

#include <cstddef>
#include "RowWindow.hpp"
#include "ShipLocker.hpp"
#include "ShipLook.hpp"

namespace rtype::client {

/** @brief What the locker shows: the chosen item, what the ship wears, the
 * items held (a cell of an item not held stays empty), and the rows shown. */
struct LockerView {
  std::size_t chosen{};
  identifiers::ShipLook worn;
  ShipLocker locker;
  RowWindow rows;
};

}  // namespace rtype::client
