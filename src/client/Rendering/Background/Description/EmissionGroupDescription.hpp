#pragma once

#include <cstddef>
#include <string>
#include "DurationRange.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Layers of particles that share one pace and one ceiling: at each
 * deadline the group draws one of its layers by their weights and a particle
 * is born there; while mostAlive particles of the group live, the deadline
 * waits for one to end.
 */
struct EmissionGroupDescription {
  std::string name;
  std::size_t mostAlive{};
  /** The time before the first birth. */
  engine::Duration first{};
  /** The time drawn between a birth and the next. */
  DurationRange every;
};

}  // namespace rtype::client
