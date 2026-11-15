#pragma once

#include <vector>

namespace rtype::client {

/** @brief The tabs of a strip as wide as their labels, and the room between
 * its two keycaps they share. */
struct TabRun {
  std::vector<int> widths;
  int room{};
};

}  // namespace rtype::client
