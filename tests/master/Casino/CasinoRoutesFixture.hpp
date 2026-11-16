#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "RouteCall.hpp"
#include "RunningMaster.hpp"

/**
 * @brief The master, started on a free port with a casino of two rooms of one
 * seat whose bets stay open ten minutes a round, the accounts ACE and R9, and
 * the token of a session of each.
 */
class CasinoRoutesFixture : public ::testing::Test {
 protected:
  CasinoRoutesFixture();

  /** @returns The token of the session of ACE. */
  [[nodiscard]] const std::string& ace() const;

  /** @returns The token of the session of R9. */
  [[nodiscard]] const std::string& secondPilot() const;

  /** @returns The answer of the master to @p call. */
  [[nodiscard]] httplib::Result send(const RouteCall& call) const;

  /** @returns The answer to the seat of the account of @p token in the first
   * room. */
  [[nodiscard]] httplib::Result takeSeat(std::string_view token) const;

  /** @returns The round of the first room whose bets stay open
   * BETS_OPEN_MARGIN more at least, waiting for the next one when the bets
   * close sooner; nothing when the master never shows one. */
  [[nodiscard]] std::optional<std::uint64_t> openRound() const;

  /** @returns The chips of the wallet of the account of @p token. */
  [[nodiscard]] std::optional<std::int64_t> chipsOf(
      std::string_view token) const;

 private:
  RunningMaster master_;
  std::string ace_;
  std::string secondPilot_;
};
