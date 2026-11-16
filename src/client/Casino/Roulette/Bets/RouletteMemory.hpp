#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "BetsReceipt.hpp"
#include "CasinoRoomState.hpp"
#include "RouletteBet.hpp"
#include "RouletteSlip.hpp"

namespace rtype::client {

/**
 * @brief What the client keeps of the roulette room while it plays there,
 * whether its screen shows or not: the bets of the round in progress, what
 * the master took of them, and the bets of the round before.
 *
 * follow() reads each state of the room: a round of another number puts the
 * bets held aside as those of the round before, and opens an empty slip; an
 * answer of the master to bets of the round in progress, read once, keeps its
 * receipt when it took them, and lays back on the slip the bets it holds when
 * it refused them. No room drops the bets of the round.
 */
class RouletteMemory {
 public:
  /** @brief Reads @p room, the state of the room followed, and @p bets, the
   * newest answer to bets laid. */
  void follow(const std::optional<network::CasinoRoomState>& room,
              const std::optional<BetsReading>& bets);

  [[nodiscard]] RouletteSlip& slip();
  [[nodiscard]] const RouletteSlip& slip() const;

  /** @returns The bets of the last round the player laid any on. */
  [[nodiscard]] const std::vector<network::RouletteBet>& previousBets() const;

  /** @returns The receipt of the bets the master took on the round in
   * progress; none before. */
  [[nodiscard]] const std::optional<network::BetsReceipt>& accepted() const;

 private:
  /** @brief Puts the bets held aside and opens the slip of @p round. */
  void startRound(std::uint64_t round);
  /** @brief Keeps what the master answered to bets on the round in
   * progress. */
  void take(const BetsReading& bets);

  std::optional<std::uint64_t> round_;
  RouletteSlip slip_;
  std::vector<network::RouletteBet> previousBets_;
  std::optional<network::BetsReceipt> accepted_;
  /** @brief The newest answer read: an answer numbered 0, given without
   * asking the master, is read every time. */
  std::uint64_t readBetsAsk_{};
};

}  // namespace rtype::client
