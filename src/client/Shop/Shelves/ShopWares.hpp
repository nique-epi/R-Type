#pragma once

#include <cstddef>
#include <vector>

namespace rtype::client {

/** @brief What the shop sells, each ware a position in
 * SHIP_LOOK_ITEM_NAMES: the looks of its locker, in the order of the
 * catalog, and the hulls of its glass case, the cheapest first. */
struct ShopWares {
  std::vector<std::size_t> looks;
  std::vector<std::size_t> hulls;
};

}  // namespace rtype::client
