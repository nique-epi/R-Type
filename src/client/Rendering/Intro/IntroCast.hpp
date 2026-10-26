#pragma once

#include <vector>
#include "IntroBurst.hpp"
#include "IntroMissile.hpp"
#include "IntroRing.hpp"

namespace rtype::client {

/**
 * @brief What the intro shows that is fixed when it starts: every missile,
 * aimed at its target; every burst, with its random sparks; every ring.
 * Where the ships and the Bydos are follows the time of the scene alone
 * (IntroShipPaths.hpp, IntroMobPaths.hpp).
 */
struct IntroCast {
  std::vector<IntroMissile> missiles;
  std::vector<IntroBurst> bursts;
  std::vector<IntroRing> rings;
};

}  // namespace rtype::client
