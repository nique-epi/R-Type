#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief The enemies of one wave: how many appear, and the time between two of
 * them appearing, in simulation time.
 */
struct Wave {
  std::size_t entityCount{};
  engine::Duration spawnInterval{};

  bool operator==(const Wave&) const = default;
};

}  // namespace rtype::game
