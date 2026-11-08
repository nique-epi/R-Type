#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json_fwd.hpp>
#include <string_view>
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "RouteCall.hpp"
#include "RunningMaster.hpp"

/**
 * @brief The master, started on a free port and a database of its own, with
 * two servers created on it, "Paris 1" and "Lyon 1", and a client of the
 * routes of the results and of the leaderboard.
 */
class LeaderboardRoutesFixture : public ::testing::Test {
 protected:
  LeaderboardRoutesFixture();

  [[nodiscard]] const rtype::network::CreatedServer& paris() const;
  [[nodiscard]] const rtype::network::CreatedServer& lyon() const;

  /** @returns The answer of the master to @p call. */
  [[nodiscard]] httplib::Result send(const RouteCall& call) const;

  /** @returns The answer to the result @p game of the game @p gameId, sent
   * with the key of @p server. */
  [[nodiscard]] httplib::Result sendResult(
      const rtype::network::CreatedServer& server, std::string_view gameId,
      const rtype::network::FinishedGame& game) const;

  /** @returns The answer to the leaderboard, @p query after its path. */
  [[nodiscard]] httplib::Result readLeaderboard(std::string_view query) const;

  /** @returns The body of readLeaderboard(), parsed; null when the master
   * did not answer. */
  [[nodiscard]] nlohmann::json leaderboardOf(std::string_view query) const;

 private:
  RunningMaster master_;
  rtype::network::CreatedServer paris_;
  rtype::network::CreatedServer lyon_;
};
