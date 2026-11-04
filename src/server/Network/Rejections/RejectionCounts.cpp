#include "RejectionCounts.hpp"
#include <cstddef>
#include "RejectionCause.hpp"

namespace rtype::server {

std::size_t countOf(const RejectionCounts& counts, RejectionCause cause) {
  return counts.at(static_cast<std::size_t>(cause));
}

void addOneTo(RejectionCounts& counts, RejectionCause cause) {
  ++counts.at(static_cast<std::size_t>(cause));
}

std::size_t totalOf(const RejectionCounts& counts) {
  std::size_t total = 0;
  for (const std::size_t count : counts) {
    total += count;
  }
  return total;
}

}  // namespace rtype::server
