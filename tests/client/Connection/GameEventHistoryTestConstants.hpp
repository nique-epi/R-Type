#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief Time between two copies of a GameEvent: the protocol page sends
 * them three times, 50 ms apart. */
constexpr engine::Duration COPY_GAP = std::chrono::milliseconds(50);

/** @brief How long the number of a GameEvent must be remembered: the protocol
 * page says the numbers of the last second are enough. */
constexpr engine::Duration EVENT_MEMORY_REQUIRED = std::chrono::seconds(1);

}  // namespace rtype::client::testing
