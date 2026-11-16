#pragma once

#include <cstdint>
#include <vector>
#include "RoulettePhase.hpp"
#include "RouletteSeat.hpp"

namespace rtype::network {

/**
 * @brief The state of a roulette table: its round, numbered from 0 and
 * compared as a serial number, its phase, the ticks left in that phase, the
 * number drawn (meaningful from Spinning on) and the players seated at the
 * table.
 */
struct RouletteRound {
  std::uint16_t round{};
  RoulettePhase phase{RoulettePhase::Betting};
  std::uint16_t ticksLeft{};
  std::uint8_t winningNumber{};
  std::vector<RouletteSeat> seats;

  bool operator==(const RouletteRound&) const = default;
};

}  // namespace rtype::network
