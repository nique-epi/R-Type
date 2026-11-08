#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief The values a parameter of a query may take, and the one it takes
 * when the request leaves it out. */
struct QueryBounds {
  std::int64_t lowest{};
  std::int64_t highest{};
  std::int64_t absent{};
};

}  // namespace rtype::master
