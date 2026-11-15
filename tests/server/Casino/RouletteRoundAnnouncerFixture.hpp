#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RouletteRound.hpp"
#include "RouletteRoundAnnouncer.hpp"

namespace rtype::server::testing {

/** @brief An announcer on a real roster, and the datagrams it sends. */
class RouletteRoundAnnouncerFixture : public ::testing::Test {
 protected:
  RouletteRoundAnnouncerFixture();

  /** @brief Seats the player at @p index in the game. */
  void join(std::size_t index);
  /** @brief Gives @p round to the announcer @p count times, one per tick. */
  void announce(const network::RouletteRound& round, std::size_t count);

  /** @returns The RouletteRound sent to the player at @p index, oldest first.
   */
  [[nodiscard]] std::vector<network::RouletteRound> roundsTo(
      std::size_t index) const;
  [[nodiscard]] RouletteRoundAnnouncer& announcer();
  /** @returns A round of bets with SOME_TICKS_LEFT that lists the player at
   * @p index alone. */
  [[nodiscard]] network::RouletteRound roundListing(std::size_t index);

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  RouletteRoundAnnouncer announcer_;
};

}  // namespace rtype::server::testing
