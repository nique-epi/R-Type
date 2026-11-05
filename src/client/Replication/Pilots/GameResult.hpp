#pragma once

#include <vector>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"

namespace rtype::client {

/** @brief How the last game ended: its pilots, best first, and whether the
 * squadron won or lost. Written by the game screen when the server ends the
 * game, read by the end screen. */
struct GameResult {
  std::vector<FinalPilot> pilots;
  network::WireGameOutcome outcome{network::WireGameOutcome::Defeat};
};

}  // namespace rtype::client
