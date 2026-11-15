#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "CasinoPlace.hpp"
#include "ConnectedPlayer.hpp"
#include "IRouletteDraw.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "TableSeat.hpp"

namespace rtype::server {

/**
 * @brief The roulette table of the waiting room of a game, shared by the
 * players who sit at it, one round after another.
 *
 * A round takes bets for ROULETTE_BETTING_TICKS, or until every seated player
 * asked to spin, then draws its number before the wheel turns for
 * ROULETTE_SPINNING_TICKS, shows the result for ROULETTE_RESULT_TICKS, and
 * opens the next round. While nobody sits, the bets stay open and the clock
 * stays full. A player who sits down is never ready; one who leaves while the
 * wheel turns stays listed, with its payout, until the round ends.
 *
 * Bets are taken only from a seated player, during the bets, for the round in
 * progress; a RouletteBets replaces the bets the player held, and one whose
 * bets are not all placeable or add up to more than
 * network::MAXIMUM_ROUND_STAKE is refused whole. What the table keeps is
 * bounded by MAXIMUM_PLAYERS times MAXIMUM_ROULETTE_BETS bets. A seat freed by
 * a departure starts afresh.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class RouletteTable {
 public:
  /**
   * @param roster The players of the game; it must outlive the table.
   * @param draw Where the numbers come from; it must outlive the table.
   */
  RouletteTable(PlayerRoster& roster, IRouletteDraw& draw);

  /** @brief Seats @p player at the table or takes it back to the room, as
   * @p place says; a seated player is not ready. */
  void onPlace(ConnectedPlayer& player, network::TablePlace place);

  /** @brief Takes @p bets of @p player in place of the ones it held, when they
   * may be taken (see the class). */
  void onBets(const ConnectedPlayer& player, const network::RouletteBets& bets);

  /** @brief Forgets the seat of @p departed, whose session closes. */
  void onPlayerClosing(const ConnectedPlayer& departed);

  /** @brief Counts a tick of the round. */
  void onTick();

  /** @brief Takes every player back to the room and forgets the rounds, as
   * the game leaves its waiting room. */
  void close();

  /** @returns The state of the table, as RouletteRound tells it. */
  [[nodiscard]] network::RouletteRound round() const;

 private:
  /** @brief Draws the number, works out each payout and turns the wheel. */
  void spin();

  /** @brief Opens the bets of the next round, the players who left forgotten.
   */
  void openNextRound();

  /** @returns Whether a player sits at the table and every one asked to spin.
   */
  [[nodiscard]] bool everySeatedPlayerVoted() const;

  /** @returns Whether a player sits at the table. */
  [[nodiscard]] bool hasSeatedPlayer() const;

  PlayerRoster& roster_;
  IRouletteDraw& draw_;
  std::array<TableSeat, network::MAXIMUM_PLAYERS> seats_{};
  std::uint16_t round_{0};
  network::RoulettePhase phase_{network::RoulettePhase::Betting};
  std::size_t ticksLeft_{0};
  std::uint8_t winningNumber_{0};
};

}  // namespace rtype::server
