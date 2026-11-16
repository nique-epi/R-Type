#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief The steps of a round of roulette: the bets are open, then the wheel
 * spins with the number already drawn, then the result shows.
 */
enum class RoulettePhase : std::uint8_t {
  Betting = 0,
  Spinning = 1,
  Result = 2,
};

}  // namespace rtype::network
