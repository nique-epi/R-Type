#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Why the shop does not sell an item to a player. */
enum class PurchaseRefusal : std::uint8_t {
  AlreadyOwned,
  NotForSale,
  TooExpensive,
};

}  // namespace rtype::client
