#pragma once

#include <vector>
#include "ChipStackLook.hpp"
#include "RouletteBet.hpp"

namespace rtype::client {

/** @brief The bets of another player seated in the room, and how its stacks
 * are drawn: tinted in its color, half clear, moved aside so the stacks of
 * the player stay readable over them. */
struct OtherPlayerBets {
  std::vector<network::RouletteBet> bets;
  ChipStackLook look;
};

}  // namespace rtype::client
