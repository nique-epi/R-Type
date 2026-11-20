#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "RouletteBet.hpp"

namespace rtype::network {

/** @brief A player seated in a roulette room, as the state of the room shows
 * it: its chips on the cloth this round, where they lie, and what the round
 * paid it. */
struct CasinoPlayer {
  std::string nickname;
  std::uint32_t stake{};
  /** @brief The chips the round handed back, 0 for a lost round; nothing
   * until the number is drawn. */
  std::optional<std::uint32_t> payout = std::nullopt;
  /** @brief Its bets on the cloth this round, so the others seated see where
   * its chips lie; empty from a master that does not list them. */
  std::vector<RouletteBet> bets;

  bool operator==(const CasinoPlayer&) const = default;
};

}  // namespace rtype::network
