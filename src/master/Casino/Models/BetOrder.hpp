#pragma once

#include <cstdint>
#include <vector>
#include "MillisecondDate.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"

namespace rtype::master {

/** @brief New bets an account sends for a round of a room: they replace every
 * bet it holds on that round. */
struct BetOrder {
  network::RoomRound place;
  std::int64_t userId{};
  std::vector<network::RouletteBet> bets;
  MillisecondDate sentAt;
};

}  // namespace rtype::master
