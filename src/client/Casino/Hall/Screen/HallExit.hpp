#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where the player walks to from the hall: the cashier, through the
 * left arch, or the roulette, through the right one. */
enum class HallExit : std::uint8_t {
  Cashier,
  Roulette,
};

}  // namespace rtype::client
