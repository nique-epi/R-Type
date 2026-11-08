#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief The item of the locker a look item gives the pilot who picks it up:
 * its number in identifiers::SHIP_LOOK_ITEM_NAMES plus one. */
struct LookItem {
  std::uint8_t number{};
};

}  // namespace rtype::game
