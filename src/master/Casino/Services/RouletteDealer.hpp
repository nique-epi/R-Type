#pragma once

#include <chrono>
#include <optional>
#include <vector>
#include "CasinoSettings.hpp"
#include "DrawnRound.hpp"
#include "IAccountWallet.hpp"
#include "IRouletteDraw.hpp"
#include "MasterDatabase.hpp"
#include "MillisecondDate.hpp"
#include "RoomRound.hpp"

namespace rtype::master {

/**
 * @brief Draws the number of each round of the casino once its bets closed,
 * and pays its winners. Each round is drawn in one transaction: its number
 * written in `casino_rounds`, and each payout added to the wallet with the
 * reference of the round, applied once. The transaction of new bets checks
 * that same row, so a bet never lands on a drawn round.
 */
class RouletteDealer {
 public:
  /** @param database, @p wallet and @p draw must outlive the dealer. */
  RouletteDealer(MasterDatabase& database, IAccountWallet& wallet,
                 network::IRouletteDraw& draw);

  /**
   * @brief Draws every round due at @p now: the round of each room of
   * @p settings once its bets closed, and every earlier round that holds bets
   * and was never drawn, such as one whose bets closed while the master was
   * down. A round already drawn is never drawn again.
   *
   * Once it drew a round, it tidies the tables: seats not seen for
   * network::CASINO_SEAT_LIFETIME, the bets of rounds drawn more than
   * DRAWN_BETS_KEPT_FOR ago, and the rounds of each room older than its last
   * network::CASINO_HISTORY_LENGTH that no kept bet names.
   * @returns The rounds it drew, oldest first.
   */
  std::vector<DrawnRound> drawDueRounds(network::MillisecondDate now,
                                        const CasinoSettings& settings);

 private:
  /** @returns The rounds due at @p now that were never drawn. */
  [[nodiscard]] std::vector<network::RoomRound> dueRounds(
      network::MillisecondDate now, const CasinoSettings& settings);

  /** @brief Draws @p place at @p now and pays its winners.
   * @returns The round drawn; nothing when it was drawn already. */
  std::optional<DrawnRound> drawn(network::RoomRound place,
                                  std::chrono::sys_seconds now);

  /** @brief Removes what the tables no longer need at @p now. */
  void tidy(std::chrono::sys_seconds now);

  MasterDatabase& database_;
  IAccountWallet& wallet_;
  network::IRouletteDraw& draw_;
};

}  // namespace rtype::master
