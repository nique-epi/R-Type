#pragma once

#include <cstdint>

namespace rtype::identifiers {

/**
 * @brief The items the ship of the player wears, one per slot: a slot holds
 * NO_SHIP_LOOK_ITEM when it wears nothing, else the position of its item in
 * SHIP_LOOK_ITEM_NAMES plus one.
 */
struct ShipLook {
  std::uint8_t head{};
  std::uint8_t eyes{};
  std::uint8_t face{};

  bool operator==(const ShipLook& other) const = default;
};

}  // namespace rtype::identifiers
