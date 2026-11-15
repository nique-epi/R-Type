#pragma once

#include <cstdint>
#include <vector>
#include "RouletteBets.hpp"

namespace rtype::server {

/**
 * @brief What the roulette table keeps of one seat of the game: whether its
 * player sits at the table, whether the round lists it, and its bets.
 */
struct TableSeat {
  /** @brief Whether the player chose the table, last it said. */
  bool seated{false};
  /** @brief Whether the round shows the seat: from the moment the player sits
   * down, until the round it left during ends. */
  bool listed{false};
  /** @brief Whether the player asked to spin now. */
  bool spinVote{false};
  std::vector<network::RouletteBet> bets;
  /** @brief The chips of bets, added up. */
  std::uint32_t stake{0};
  /** @brief What the bets return once the number is drawn; 0 before. */
  std::uint32_t payout{0};
};

}  // namespace rtype::server
