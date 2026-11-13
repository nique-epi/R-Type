#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief How many wares each shelf of the shop holds. */
struct ShelfSizes {
  std::size_t looks{};
  std::size_t hulls{};
};

}  // namespace rtype::client
