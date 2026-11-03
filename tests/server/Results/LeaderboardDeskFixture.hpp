#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "LeaderboardDesk.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RecordingResultsArchive.hpp"

namespace rtype::server::testing {

/** @brief A desk on a real roster, an archive that records, and the datagrams
 * it sends. */
class LeaderboardDeskFixture : public ::testing::Test {
 protected:
  LeaderboardDeskFixture();

  /** @brief Seats the player at @p index, under a token of its own. */
  ConnectedPlayer& seat(std::size_t index);
  /** @brief Frees the seat of the player at @p index, and tells the desk, as a
   * session that closes. */
  void leave(std::size_t index);
  /** @brief Seats the player at @p index again, from the same address, under a
   * token no earlier session had. */
  ConnectedPlayer& seatAgainUnderANewToken(std::size_t index);
  /** @brief The player asks for a leaderboard. */
  void ask(const ConnectedPlayer& player);
  void runTicks(std::size_t count);

  [[nodiscard]] RecordingResultsArchive& archive();
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const;

 private:
  PlayerRoster roster_;
  RecordingResultsArchive archive_;
  std::vector<network::OutgoingDatagram> sent_;
  LeaderboardDesk desk_;
};

}  // namespace rtype::server::testing
