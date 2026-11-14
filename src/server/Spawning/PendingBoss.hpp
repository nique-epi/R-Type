#pragma once

#include <cstddef>
#include "EntitySheet.hpp"
#include "PercentConstants.hpp"

namespace rtype::server {

/** @brief A boss a level announced that has not entered yet: its sheet, its
 * health in percent of that sheet, and the ticks left of its warning. */
struct PendingBoss {
  const game::EntitySheet* sheet{nullptr};
  int healthPercent{game::WHOLE_PERCENT};
  std::size_t ticksLeft{0};
};

}  // namespace rtype::server
