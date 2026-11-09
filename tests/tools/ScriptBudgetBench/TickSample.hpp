#pragma once

#include "TimeConstants.hpp"

namespace rtype::tools {

/** @brief What one tick of the bench took. */
struct TickSample {
  /** @brief The whole tick, its scripts included. */
  engine::Duration whole{};
  /** @brief The part of it spent in the scripts. */
  engine::Duration scripts{};
};

}  // namespace rtype::tools
