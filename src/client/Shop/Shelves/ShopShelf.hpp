#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where the shop shows a ware: a cell of its locker, for a look, or
 * a stall of its glass case, for a hull. */
enum class ShopShelf : std::uint8_t {
  Locker,
  HullCase,
};

}  // namespace rtype::client
