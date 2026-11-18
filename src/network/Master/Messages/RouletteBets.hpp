#pragma once

#include <vector>
#include "RouletteBet.hpp"

namespace rtype::network {

/**
 * @brief The body of `PUT /api/casino/rooms/{room}/rounds/{round}/bets/me`:
 * every bet a player holds on the cloth this round, which replace those it
 * held; none takes them all back.
 */
struct RouletteBets {
  std::vector<RouletteBet> bets;

  bool operator==(const RouletteBets&) const = default;
};

}  // namespace rtype::network
