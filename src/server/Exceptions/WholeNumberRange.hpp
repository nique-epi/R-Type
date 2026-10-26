#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief The whole numbers from lowest to highest, both included. */
struct WholeNumberRange {
  std::int64_t lowest{};
  std::int64_t highest{};
};

}  // namespace rtype::server
