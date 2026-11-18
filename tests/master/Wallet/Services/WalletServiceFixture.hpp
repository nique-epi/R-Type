#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include "ChipMovement.hpp"
#include "ChipMovementResult.hpp"
#include "Currency.hpp"
#include "GameCredit.hpp"
#include "GameTicketService.hpp"
#include "MasterDatabase.hpp"
#include "MasterDatabaseFile.hpp"
#include "WalletService.hpp"

/** @brief The wallets of the master on a database of their own, holding the
 * account ACE, whose wallet opens with TEST_STARTING_WALLET. */
class WalletServiceFixture : public ::testing::Test {
 protected:
  WalletServiceFixture();

  /** @returns The wallets under test. */
  [[nodiscard]] rtype::master::WalletService& wallets();

  /** @returns The database they keep their tables in. */
  [[nodiscard]] rtype::master::MasterDatabase& database();

  /** @returns The id of the account ACE. */
  [[nodiscard]] std::int64_t ace() const;

  /** @returns The sum of the lines of the journal of ACE in @p currency. */
  [[nodiscard]] std::int64_t journalSumOf(rtype::network::Currency currency);

  /** @brief Makes ACE no longer own the look named @p item. */
  void forgetLook(std::string_view item);

  /** @returns A stake of @p amount chips of ACE, negative, applied as often
   * as asked. */
  [[nodiscard]] rtype::master::ChipMovement stakeOf(std::int64_t amount) const;

  /** @returns What @p movement did, applied in a transaction of its own. */
  rtype::master::ChipMovementResult applied(
      const rtype::master::ChipMovement& movement);

  /** @returns A ticket the master gives ACE at @p issuedAt. */
  [[nodiscard]] std::string aceTicketAt(std::chrono::sys_seconds issuedAt);

  /** @returns The credit of the game of GAME_REFERENCE through @p ticket at
   * WALLET_TEST_DATE: its kopecks, the crown, and a name of no look. */
  [[nodiscard]] static rtype::master::GameCredit gameCreditOf(
      const std::string& ticket);

 private:
  MasterDatabaseFile file_;
  std::int64_t ace_{};
  rtype::master::WalletService wallets_;
  rtype::master::GameTicketService tickets_;
};
