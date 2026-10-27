#pragma once

#include <array>
#include <cstddef>
#include "EntitySpriteConstants.hpp"
#include "PlayerColors.hpp"

namespace rtype::client {

/** @brief The shades of every player color, player 1 first: those of the
 * sheets (PLAYER_COLORS), or those of a palette made for a form of color
 * blindness. */
using PlayerPalette =
    std::array<PlayerColors, static_cast<std::size_t>(PLAYER_COLOR_COUNT)>;

}  // namespace rtype::client
