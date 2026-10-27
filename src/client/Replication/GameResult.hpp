#pragma once

#include <vector>
#include "FinalPilot.hpp"

namespace rtype::client {

/** @brief How the last game ended: its pilots, best first. Written by the
 * game screen when the server ends the game, read by the end screen. */
struct GameResult {
  std::vector<FinalPilot> pilots;
};

}  // namespace rtype::client
