#pragma once

#include <cstdint>
#include <vector>
#include "RouletteBet.hpp"

namespace rtype::network {

/** @brief The answer of `PUT /api/casino/rooms/{room}/rounds/{round}/bets/me`:
 * the bets now on the cloth, the chips they hold, and the chips of the wallet
 * once they are staked. */
struct BetsReceipt {
  std::uint64_t round{};
  std::vector<RouletteBet> bets;
  std::uint32_t stake{};
  std::uint32_t chips{};

  bool operator==(const BetsReceipt&) const = default;
};

}  // namespace rtype::network
