#pragma once

#include <string>
#include <vector>
#include "WaveCurve.hpp"

namespace rtype::server {

/**
 * @brief The waves of one difficulty: how they grow, and the names of the
 * entity sheets their enemies take in turn, the cycle carrying on from one
 * wave to the next.
 */
struct WavePlan {
  game::WaveCurve curve;
  std::vector<std::string> enemies;

  bool operator==(const WavePlan&) const = default;
};

}  // namespace rtype::server
