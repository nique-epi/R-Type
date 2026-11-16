#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <variant>
#include <vector>
#include "BetRefusal.hpp"
#include "BetsReceipt.hpp"
#include "CasinoService.hpp"
#include "DrawnRound.hpp"
#include "MasterDatabaseFile.hpp"
#include "MillisecondDate.hpp"
#include "RouletteDealer.hpp"
#include "ScriptedRouletteDraw.hpp"
#include "WalletService.hpp"

/**
 * @brief The casino of DEALER_TEST_SETTINGS on a database of its own, with
 * real wallets that open with TEST_STARTING_WALLET, the account ACE, and a
 * dealer whose draw gives RED_NINETEEN, then ZERO.
 */
class RouletteDealerFixture : public ::testing::Test {
 protected:
  RouletteDealerFixture();

  [[nodiscard]] rtype::master::CasinoService& casino();
  /** @returns The id of the account ACE. */
  [[nodiscard]] std::int64_t ace() const;
  [[nodiscard]] rtype::network::testing::ScriptedRouletteDraw& draw();
  [[nodiscard]] rtype::master::RouletteDealer& dealer();

  /** @returns What ACE laying 10 chips on 19 and 25 on red in BET_ROOM, on
   * the round of @p sentAt, gave; ACE takes its seat first. */
  std::variant<rtype::network::BetsReceipt, rtype::master::BetRefusal>
  aceBetsAt(rtype::network::MillisecondDate sentAt);

  /** @returns The rounds the dealer drew at @p now. */
  std::vector<rtype::master::DrawnRound> drawAt(
      rtype::network::MillisecondDate now);

  /** @returns What the dealer of the tests draws at @p now, the rounds it
   * missed included, as the first pass of a job does. */
  [[nodiscard]] std::vector<rtype::master::DrawnRound> drawMissedAt(
      rtype::network::MillisecondDate now);

  /** @returns The chips of the wallet of ACE. */
  [[nodiscard]] std::uint32_t chipsOfAce();

 private:
  MasterDatabaseFile file_;
  std::int64_t ace_{};
  rtype::master::WalletService wallets_;
  rtype::network::testing::ScriptedRouletteDraw draw_;
  rtype::master::CasinoService casino_;
  rtype::master::RouletteDealer dealer_;
};
