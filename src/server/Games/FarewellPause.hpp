#pragma once

#include <functional>
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief Waits for the given time between two copies of the farewell; the
 * server sleeps, a test only writes the time down. */
using FarewellPause = std::function<void(engine::Duration)>;

}  // namespace rtype::server
