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
 * reference of the round. That transaction draws nothing when the row is
 * there already, so a round is paid once; the transaction of new bets checks
 * that same row, so a bet never lands on a drawn round.
 */
class RouletteDealer {
 public:
  /** @param database, @p wallet and @p draw must outlive the dealer. */
  RouletteDealer(MasterDatabase& database, IAccountWallet& wallet,
                 network::IRouletteDraw& draw);

  /**
   * @brief Draws the round of each room of @p settings whose bets closed at
   * @p now, unless it was drawn already: the pass a running master makes
   * every interval, which reads which rooms were drawn in one query.
   *
   * Once it drew a round, it tidies the tables: seats not seen for
   * network::CASINO_SEAT_LIFETIME, the bets of rounds drawn more than
   * DRAWN_BETS_KEPT_FOR ago, and the rounds of each room older than its last
   * network::CASINO_HISTORY_LENGTH that no kept bet names.
   * @returns The rounds it drew, oldest first.
   */
  std::vector<DrawnRound> drawDueRounds(network::MillisecondDate now,
                                        const CasinoSettings& settings);

  /**
   * @brief Draws what drawDueRounds() draws, and every earlier round that
   * holds bets and was never drawn, such as one whose bets closed while the
   * master was down: the pass of a start, and of a pass after one that failed.
   * It tidies the tables as drawDueRounds() does.
   * @returns The rounds it drew, oldest first.
   */
  std::vector<DrawnRound> drawMissedRounds(network::MillisecondDate now,
                                           const CasinoSettings& settings);

 private:
  /** @brief Draws each of @p places at @p now, then tidies the tables once
   * it drew one.
   * @returns The rounds it drew. */
  std::vector<DrawnRound> drawnEach(
      const std::vector<network::RoomRound>& places,
      network::MillisecondDate now);

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
