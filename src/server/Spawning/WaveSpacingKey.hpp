#pragma once

#include <cstdint>
#include <string_view>
#include "WholeNumberRange.hpp"

namespace rtype::server {

/** @brief An optional key of a difficulty that spaces what its waves bring:
 * its name, the waves it gives when left out, and the range it accepts. */
struct WaveSpacingKey {
  std::string_view key;
  std::int64_t fallback{};
  engine::WholeNumberRange range{};
};

}  // namespace rtype::server
