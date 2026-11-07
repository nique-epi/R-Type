#pragma once

#include <cstddef>
#include "ShipLook.hpp"

namespace rtype::client {

/** @brief What the locker shows: the chosen item, and what the ship wears. */
struct LockerView {
  std::size_t chosen{};
  identifiers::ShipLook worn;
};

}  // namespace rtype::client
