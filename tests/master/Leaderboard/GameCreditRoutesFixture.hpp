#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "RunningMaster.hpp"

/**
 * @brief The master, started on a free port with its wallets, the server
 * "Paris 1", the accounts ACE and BOB, and the token of a session of each.
 */
class GameCreditRoutesFixture : public ::testing::Test {
 protected:
  GameCreditRoutesFixture();

  /** @returns The id of the account ACE, as decimal text. */
  [[nodiscard]] const std::string& aceAccountId() const;

  /** @returns A game ticket the master gives ACE. */
  [[nodiscard]] std::string aceTicket() const;

  /** @returns A game ticket the master gives BOB. */
  [[nodiscard]] std::string bobTicket() const;

  /** @returns The answer to the result @p game of the game @p gameId, sent by
   * "Paris 1". */
  [[nodiscard]] httplib::Result sendResult(
      std::string_view gameId, const rtype::network::FinishedGame& game) const;

  /** @returns The answer to the leaderboard of the accounts, @p query after
   * its path. */
  [[nodiscard]] httplib::Result readAccountLeaderboard(
      std::string_view query) const;

  /** @brief Sends two games paid through tickets of ACE: ACE scoring 1000,
   * then ACE_2 scoring 1234. */
  void sendAceUnderTwoNicknames() const;

  /** @returns The global leaderboard @p query reads, parsed. */
  [[nodiscard]] nlohmann::json leaderboardOf(std::string_view query) const;

  /** @returns The kopecks of the wallet of ACE. */
  [[nodiscard]] std::optional<std::int64_t> kopecksOfAce() const;

  /** @returns The kopecks of the wallet of BOB. */
  [[nodiscard]] std::optional<std::int64_t> kopecksOfBob() const;

  /** @returns How many looks ACE owns. */
  [[nodiscard]] std::optional<std::size_t> looksOfAce() const;

 private:
  RunningMaster master_;
  rtype::network::CreatedServer paris_;
  std::string aceToken_;
  std::string bobToken_;
  std::string aceAccountId_;
};
