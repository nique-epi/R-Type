#pragma once

#include <cstdint>
#include "MillisecondDate.hpp"
#include "RoulettePhase.hpp"

namespace rtype::master {

/**
 * @brief Where the clock stands in the rounds of the casino: the round played
 * now, its phase, and the dates its phases end at. The bets are open before
 * closesAt, the wheel spins from closesAt to resultAt, the result shows from
 * resultAt to endsAt, where the next round starts.
 */
struct ScheduledRound {
  std::uint64_t round{};
  network::RoulettePhase phase{network::RoulettePhase::Betting};
  MillisecondDate closesAt;
  MillisecondDate resultAt;
  MillisecondDate endsAt;
};

}  // namespace rtype::master
