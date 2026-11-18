#pragma once

#include <cstdint>
#include <optional>

namespace rtype::client {

/** @brief What the casino shows of the purse of the player, top right: the
 * kopecks and the chips; nothing for a balance the master has not answered,
 * shown as UNKNOWN_KOPECKS_TEXT. */
struct CasinoBalances {
  std::optional<std::uint32_t> kopecks;
  std::optional<std::uint32_t> chips;
};

}  // namespace rtype::client
