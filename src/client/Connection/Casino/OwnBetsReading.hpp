#pragma once

#include <cstdint>
#include <optional>
#include "RoomRound.hpp"
#include "RoundBets.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for the bets of the player on a
 * round: nothing when it did not answer, refused, or answered what the client
 * cannot read. */
struct OwnBetsReading {
  /** @brief The number of the ask it answers (see CasinoAsks). */
  std::uint64_t ask{};
  network::RoomRound round;
  std::optional<network::RoundBets> bets;
};

}  // namespace rtype::client
