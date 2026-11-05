#pragma once

#include <functional>
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief Lets @p pause of time pass: a sleep in the supervisor, a counter in
 * the tests. */
using StopPause = std::function<void(engine::Duration pause)>;

}  // namespace rtype::server
