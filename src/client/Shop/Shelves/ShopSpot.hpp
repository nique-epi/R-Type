#pragma once

#include <cstddef>
#include "ShopShelf.hpp"

namespace rtype::client {

/** @brief A ware of the shop: its shelf, and its place on it, 0 for the
 * first. */
struct ShopSpot {
  ShopShelf shelf{};
  std::size_t place{};

  bool operator==(const ShopSpot& other) const = default;
};

}  // namespace rtype::client
