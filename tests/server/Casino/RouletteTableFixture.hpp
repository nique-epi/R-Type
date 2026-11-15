#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "PlayerRoster.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "RouletteTable.hpp"
#include "ScriptedRouletteDraw.hpp"

namespace rtype::server::testing {

/** @brief A table on a real roster, whose numbers come from a script that
 * always gives SCRIPTED_NUMBER. */
class RouletteTableFixture : public ::testing::Test {
 protected:
  RouletteTableFixture();

  /** @brief Seats the player at @p index in the game, under a token of its own.
   */
  ConnectedPlayer& join(std::size_t index);
  /** @brief The player at @p index sits at the table. */
  void sit(std::size_t index);
  /** @brief The player at @p index goes back to the room. */
  void standUp(std::size_t index);
  /** @brief The player at @p index sends @p bets for the round in progress,
   * with @p spinVote. */
  void bet(std::size_t index, const std::vector<network::RouletteBet>& bets,
           bool spinVote);
  /** @brief Frees the seat of the player at @p index, and tells the table, as a
   * session that closes. */
  void leave(std::size_t index);
  void runTicks(std::size_t count);

  /** @returns The seat the round lists for the player at @p index, if any. */
  [[nodiscard]] std::optional<network::RouletteSeat> listedSeat(
      std::size_t index);
  [[nodiscard]] ConnectedPlayer& player(std::size_t index);
  [[nodiscard]] RouletteTable& table();
  [[nodiscard]] const ScriptedRouletteDraw& draw() const;

 private:
  PlayerRoster roster_;
  ScriptedRouletteDraw draw_;
  RouletteTable table_;
};

}  // namespace rtype::server::testing
