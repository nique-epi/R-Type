#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::engine::testing {

/** @brief One second of simulation time, for the tests that measure what moves
 * or changes in that time. */
constexpr Duration ONE_SECOND = std::chrono::seconds(1);

}  // namespace rtype::engine::testing
