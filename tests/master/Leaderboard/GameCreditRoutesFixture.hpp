#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "RunningMaster.hpp"

/**
 * @brief The master, started on a free port with its wallets, the server
 * "Paris 1", the account ACE and the token of a session of ACE.
 */
class GameCreditRoutesFixture : public ::testing::Test {
 protected:
  GameCreditRoutesFixture();

  /** @returns The id of the account ACE, as decimal text. */
  [[nodiscard]] const std::string& aceAccountId() const;

  /** @returns The answer to the result @p game of the game @p gameId, sent by
   * "Paris 1". */
  [[nodiscard]] httplib::Result sendResult(
      std::string_view gameId, const rtype::network::FinishedGame& game) const;

  /** @returns The kopecks of the wallet of ACE. */
  [[nodiscard]] std::optional<std::int64_t> kopecksOfAce() const;

  /** @returns How many looks ACE owns. */
  [[nodiscard]] std::optional<std::size_t> looksOfAce() const;

 private:
  RunningMaster master_;
  rtype::network::CreatedServer paris_;
  std::string token_;
  std::string aceAccountId_;
};
