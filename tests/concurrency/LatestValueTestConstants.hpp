#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace rtype::concurrency::testing {

/** @brief The values the tests publish, one after the other. */
constexpr std::size_t OLDER_VALUE = 2;
constexpr std::size_t NEWER_VALUE = 3;

/** @brief How many values the writer thread of a test publishes, and how many
 * times the reader reads meanwhile. */
constexpr std::size_t VALUES_PUBLISHED = 20000;
constexpr std::size_t TIMES_READ = 20000;

/** @brief The longest value of those tests, so copying one really reads a heap
 * block a publication can free. */
constexpr std::size_t LONGEST_VALUE = 16;

/** @brief A wait that ends on its own, and one long enough that only a
 * publication or a stop ends it within the time of a test. */
constexpr std::chrono::milliseconds SHORT_WAIT{20};
constexpr std::chrono::seconds LONG_WAIT{30};

/** @brief The revision before any publication, and after the first one. */
constexpr std::uint64_t NO_REVISION = 0;
constexpr std::uint64_t FIRST_REVISION = 1;

}  // namespace rtype::concurrency::testing
