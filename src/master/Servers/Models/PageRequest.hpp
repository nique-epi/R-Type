#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief The items of a list a request reads: limit of them at most, after
 * the first offset. */
struct PageRequest {
  std::int64_t limit{};
  std::int64_t offset{};
};

}  // namespace rtype::master
