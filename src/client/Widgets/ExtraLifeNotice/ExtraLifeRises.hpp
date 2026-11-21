#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include "EntitySpriteConstants.hpp"

namespace rtype::client {

/** @brief How far the notice of an extra life over each bay, from the left,
 * has risen, in rows; nothing for a bay that shows none. */
using ExtraLifeRises = std::array<std::optional<int>,
                                  static_cast<std::size_t>(PLAYER_COLOR_COUNT)>;

}  // namespace rtype::client
