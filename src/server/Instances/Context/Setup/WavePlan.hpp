#pragma once

#include <vector>
#include "BydoKind.hpp"
#include "WaveCurve.hpp"

namespace rtype::server {

/**
 * @brief The waves of one difficulty: how they grow, and the kinds their Bydos
 * take in turn, the cycle carrying on from one wave to the next.
 */
struct WavePlan {
  game::WaveCurve curve;
  std::vector<game::BydoKind> enemies;

  bool operator==(const WavePlan&) const = default;
};

}  // namespace rtype::server
