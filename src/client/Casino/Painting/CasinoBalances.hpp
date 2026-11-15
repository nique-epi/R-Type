#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What the casino shows of the purse of the player, top right: the
 * kopecks and the chips. */
struct CasinoBalances {
  std::uint32_t kopecks{};
  std::uint32_t chips{};
};

}  // namespace rtype::client
