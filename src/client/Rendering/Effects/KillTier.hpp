#pragma once

#include <optional>
#include "Burst.hpp"
#include "Ring.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What a Bydo destroyed plays at its tier: how much longer its
 * explosion holds its first frame, its drops, its shockwave if any, and the
 * trauma it adds to the camera shake.
 */
struct KillTier {
  engine::Duration hold{};
  Burst drops;
  std::optional<Ring> ring;
  float trauma{};
};

}  // namespace rtype::client
