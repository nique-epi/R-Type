#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief The weapon a ship holds, as its Gun names it: 0 for the gun of its
 * sheet, n for the n-th weapon of its sheet, and its level, from 1; 0 with
 * the gun of the sheet. */
struct HeldWeapon {
  std::uint8_t weapon{0};
  std::uint8_t level{0};
};

}  // namespace rtype::game
