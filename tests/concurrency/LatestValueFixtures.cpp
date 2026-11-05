#include "LatestValueFixtures.hpp"
#include <cstddef>
#include <vector>
#include "LatestValueTestConstants.hpp"

namespace rtype::concurrency::testing {

std::vector<std::size_t> valueCounting(std::size_t count) {
  return std::vector<std::size_t>(count % LONGEST_VALUE, count);
}

}  // namespace rtype::concurrency::testing
