#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameEndAnnouncer.hpp"
#include "GameEnded.hpp"
#include "GameOutcome.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server::testing {

/**
 * @brief A roster, the announcer of the end of its game once the test ends
 * it, and the datagrams the announcer handed to its send function, in order.
 */
class GameEndAnnouncerFixture : public ::testing::Test {
 protected:
  /**
   * @brief Connects the player number @p index + 1, under SESSION_TOKEN.
   * @returns The player, who stays in the roster.
   */
  ConnectedPlayer& join(std::size_t index);

  /** @brief Ends the game with @p outcome: builds the announcer, which reads
   * the result from the roster now. */
  void endTheGame(GameOutcome outcome = GameOutcome::Defeat);

  /** @brief Lets @p count ticks of the finished game pass. */
  void runFinishedTicks(std::size_t count);

  /** @returns The datagrams sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
      std::size_t index) const;

  /** @returns The result of a game the first and the second player end with
   * FIRST_PLAYER_FINAL_SCORE and SECOND_PLAYER_FINAL_SCORE, in @p outcome. */
  [[nodiscard]] static network::GameEnded resultOfBothPlayers(
      network::WireGameOutcome outcome = network::WireGameOutcome::Defeat);

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  std::unique_ptr<GameEndAnnouncer> announcer_;
};

}  // namespace rtype::server::testing
