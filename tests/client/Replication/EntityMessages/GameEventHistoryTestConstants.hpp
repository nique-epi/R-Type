#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief How long the number of a GameEvent must be remembered: the protocol
 * page says the numbers of the last 2 s are enough, the lifetime of a
 * guaranteed message (1 s) and 1 s for copies whose delays differ. */
constexpr engine::Duration EVENT_MEMORY_REQUIRED = std::chrono::seconds(2);

}  // namespace rtype::client::testing
