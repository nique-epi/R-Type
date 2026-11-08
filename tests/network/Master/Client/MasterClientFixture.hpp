#pragma once

#include <gtest/gtest.h>
#include <functional>
#include <string>
#include "CreatedServer.hpp"
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "MasterClient.hpp"
#include "NetworkException.hpp"
#include "RunningMaster.hpp"

/** @brief A running master, and clients that call it as a game server does. */
class MasterClientFixture : public ::testing::Test {
 protected:
  /** @returns A client of the master that sends @p key. */
  [[nodiscard]] rtype::network::MasterClient clientWith(
      const std::string& key) const;

  [[nodiscard]] const RunningMaster& master() const;

  /** @returns The server "Paris 1", created through a client with no key. */
  [[nodiscard]] rtype::network::CreatedServer createdParis() const;

  /** @returns The query of the leaderboard the game of parisFinishedGame()
   * counts in, BYDO_HUNTER named as its player. */
  [[nodiscard]] static rtype::network::GlobalLeaderboardQuery
  parisGameLeaderboardQuery();

  /** @returns That leaderboard once @p paris sent the game alone: ACE first,
   * BYDO_HUNTER second and as the player asked for. */
  [[nodiscard]] static rtype::network::GlobalLeaderboard parisGameLeaderboardOf(
      const rtype::network::CreatedServer& paris);

  /**
   * @returns The MasterRefusedException @p call throws, or a refusal of
   * NO_REFUSAL status when it throws none.
   */
  [[nodiscard]] static rtype::network::MasterRefusedException refusalOf(
      const std::function<void()>& call);

 private:
  RunningMaster master_;
};
