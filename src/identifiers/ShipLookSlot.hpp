#pragma once

#include <cstdint>

namespace rtype::identifiers {

/** @brief Where an item sits on the ship; a slot wears one item at most. Hull
 * is the ship itself: an item of that slot replaces its picture and hides
 * nothing of what the other slots wear. */
enum class ShipLookSlot : std::uint8_t {
  Head,
  Eyes,
  Face,
  Hull,
};

}  // namespace rtype::identifiers
