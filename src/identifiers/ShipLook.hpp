#pragma once

#include <cstdint>

namespace rtype::identifiers {

/**
 * @brief The items the ship of the player wears, one per slot: a slot holds
 * NO_SHIP_LOOK_ITEM when it wears nothing, else the position of its item in
 * SHIP_LOOK_ITEM_NAMES plus one. hull names the ship itself:
 * NO_SHIP_LOOK_ITEM is the standard ship.
 */
struct ShipLook {
  std::uint8_t head{};
  std::uint8_t eyes{};
  std::uint8_t face{};
  std::uint8_t hull{};

  bool operator==(const ShipLook& other) const = default;
};

}  // namespace rtype::identifiers
