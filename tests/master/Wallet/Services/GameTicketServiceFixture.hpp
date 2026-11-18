#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include "GameTicketService.hpp"
#include "MasterDatabaseFile.hpp"

/** @brief The game tickets of the master on a database of their own, holding
 * the account ACE. */
class GameTicketServiceFixture : public ::testing::Test {
 protected:
  GameTicketServiceFixture();

  /** @returns The tickets under test. */
  [[nodiscard]] rtype::master::GameTicketService& tickets();

  /** @returns The id of the account ACE. */
  [[nodiscard]] std::int64_t ace() const;

  /** @returns How many rows of `game_tickets` hold @p fingerprint as their
   * fingerprint. */
  [[nodiscard]] std::int64_t rowsKeyedBy(const std::string& fingerprint);

 private:
  MasterDatabaseFile file_;
  std::int64_t ace_{};
  rtype::master::GameTicketService tickets_;
};
