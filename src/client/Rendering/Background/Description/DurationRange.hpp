#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The durations from shortest to longest, both included. */
struct DurationRange {
  engine::Duration shortest{};
  engine::Duration longest{};
};

}  // namespace rtype::client
