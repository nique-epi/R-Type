#pragma once

#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief How long one system of a SystemScheduler ran, in total, since the
 * scheduler was last asked.
 */
struct SystemTime {
  /** @brief The name the system was added under, empty when it was given none.
   * The text outlives the scheduler. */
  std::string_view name;
  Duration total{};
};

}  // namespace rtype::engine
