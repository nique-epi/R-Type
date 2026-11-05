#pragma once

#include <chrono>

namespace rtype::concurrency::testing {

/** @brief A sleep that ends on its own within a test, and one long enough that
 * only a stop request ends it within the time of a test. */
constexpr std::chrono::milliseconds SHORT_SLEEP{20};
constexpr std::chrono::milliseconds LONG_SLEEP{30'000};

}  // namespace rtype::concurrency::testing
