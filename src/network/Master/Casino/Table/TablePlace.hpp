#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Where a player stands: away from the roulette table, or seated at
 * it.
 */
enum class TablePlace : std::uint8_t {
  InRoom = 0,
  AtTable = 1,
};

}  // namespace rtype::network
