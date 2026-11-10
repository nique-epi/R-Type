#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::tools {

/** @brief What one tick of the bench took. */
struct TickSample {
  /** @brief The whole tick, its scripts included. */
  engine::Duration whole{};
  /** @brief The part of it spent in the scripts. */
  engine::Duration scripts{};
  /** @brief The part of it spent in CollisionSystem, publishing its collisions
   * included. */
  engine::Duration collisions{};
  /** @brief The collisions CollisionSystem published in it. */
  std::size_t collisionsFound{0};
};

}  // namespace rtype::tools
