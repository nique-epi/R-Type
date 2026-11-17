#pragma once

#include <SQLiteCpp/Database.h>
#include "ChipMovement.hpp"
#include "ChipMovementResult.hpp"
#include "GameCredit.hpp"
#include "GameCreditOutcome.hpp"

namespace rtype::master {

/**
 * @brief What the other modules of the master change in the wallets of the
 * accounts: the chips the casino stakes and pays, and the kopecks and looks a
 * game pays. The module wallet provides it; it is compiled without that
 * module, so a module that reads it with find() works without it.
 */
class IAccountWallet {
 public:
  virtual ~IAccountWallet() = default;

  /**
   * @brief Applies @p movement on @p connection, inside the transaction the
   * caller opened with MasterDatabase::transact(): the balance and its line
   * of the journal are written with the writes of the caller, or not at all.
   * It never takes the lock of the database, which the caller holds.
   * @returns The chips after it, or why it was refused.
   */
  [[nodiscard]] virtual ChipMovementResult applyChips(
      SQLite::Database& connection, const ChipMovement& movement) = 0;

  /**
   * @brief Credits @p credit, in one transaction of its own, to the account
   * the master gave its ticket to, if that ticket was never used and has not
   * expired: marks the ticket used, then writes the kopecks, the looks won,
   * and the reference, so it is never credited twice. A ticket pays one
   * result.
   * @returns What it did.
   */
  virtual GameCreditOutcome creditGame(const GameCredit& credit) = 0;
};

}  // namespace rtype::master
