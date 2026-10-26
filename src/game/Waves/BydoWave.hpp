#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief The Bydos of one wave: how many appear, and the time between two of
 * them appearing, in simulation time.
 */
struct BydoWave {
  std::size_t bydoCount{};
  engine::Duration spawnInterval{};

  bool operator==(const BydoWave&) const = default;
};

}  // namespace rtype::game
