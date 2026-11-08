#pragma once

#include <cstdint>

namespace rtype::identifiers {

/** @brief Where an item sits on the ship; a slot wears one item at most. */
enum class ShipLookSlot : std::uint8_t {
  Head,
  Eyes,
  Face,
};

}  // namespace rtype::identifiers
