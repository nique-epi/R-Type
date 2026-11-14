#pragma once

#include <cstdint>
#include "ShipLocker.hpp"

namespace rtype::client {

/** @brief What the player buys with and what they bought: the balance, in
 * kopecks, the chips they bet at the casino, and the locker. */
struct Purse {
  std::uint32_t kopecks{};
  std::uint32_t chips{};
  ShipLocker locker;

  bool operator==(const Purse& other) const = default;
};

}  // namespace rtype::client
