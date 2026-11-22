#pragma once

#include <cstdint>

namespace rtype::network {

/** @brief What a pilot ends a game with that pays kopecks: its final score,
 * and the bounty of the bosses destroyed and the level cleared while it was
 * there. */
struct GameGain {
  std::uint32_t score{0};
  std::uint32_t bounty{0};
};

}  // namespace rtype::network
