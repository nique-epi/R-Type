#pragma once

#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief One attack of an enemy and how long each of its phases lasts, as the
 * settings of its behaviour give them. A phase of zero is skipped: an attack
 * with no signal starts on its strike.
 */
struct ActionTimings {
  std::uint8_t attack{0};
  engine::Duration signal{};
  engine::Duration strike{};
  engine::Duration recover{};
};

}  // namespace rtype::game
