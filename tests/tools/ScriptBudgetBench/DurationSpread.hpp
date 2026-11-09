#pragma once

#include "TimeConstants.hpp"

namespace rtype::tools {

/** @brief How long a kind of work took over the ticks of a run. */
struct DurationSpread {
  engine::Duration median{};
  /** @brief What 99 ticks out of 100 stayed under. */
  engine::Duration high{};
  engine::Duration worst{};
};

}  // namespace rtype::tools
