#pragma once

#include <cstddef>
#include <vector>

namespace rtype::concurrency::testing {

/** @returns @p count % LONGEST_VALUE copies of @p count, so a reader can tell a
 * whole value from a torn one: every element equals the count it was built
 * from. */
[[nodiscard]] std::vector<std::size_t> valueCounting(std::size_t count);

}  // namespace rtype::concurrency::testing
