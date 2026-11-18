#pragma once

#include <chrono>

namespace rtype::master {

/** @brief How long each phase of a round of roulette lasts: the bets are open,
 * then the wheel spins, then the result shows. */
struct RouletteDurations {
  std::chrono::milliseconds betting{};
  std::chrono::milliseconds spinning{};
  std::chrono::milliseconds result{};
};

}  // namespace rtype::master
