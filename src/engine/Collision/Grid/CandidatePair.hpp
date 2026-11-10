#pragma once

#include <cstddef>

namespace rtype::engine {

/**
 * @brief Two boxes of a UniformGrid that share at least one cell, numbered in
 * the order they were inserted since the grid was last cleared, the earlier
 * one first.
 *
 * Sharing a cell is not overlapping: the caller still tests the two boxes with
 * overlaps().
 */
struct CandidatePair {
  std::size_t first{};
  std::size_t second{};

  bool operator==(const CandidatePair&) const = default;
};

}  // namespace rtype::engine
