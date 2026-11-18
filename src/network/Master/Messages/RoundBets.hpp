#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "RouletteBet.hpp"

namespace rtype::network {

/** @brief The answer of `GET /api/casino/rooms/{room}/rounds/{round}/bets/me`:
 * the bets a player holds on a round, and what the round paid them once it
 * was drawn. */
struct RoundBets {
  std::uint64_t round{};
  std::vector<RouletteBet> bets;
  std::uint32_t stake{};
  /** @brief The chips the bets handed back, 0 for a lost round; nothing
   * until the number is drawn. */
  std::optional<std::uint32_t> payout = std::nullopt;
  /** @brief The number drawn; nothing until it is. */
  std::optional<std::uint8_t> winningNumber = std::nullopt;

  bool operator==(const RoundBets&) const = default;
};

}  // namespace rtype::network
