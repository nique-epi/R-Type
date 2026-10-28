#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include <cstddef>
#include "EffectConstants.hpp"
#include "EntitySpriteConstants.hpp"

namespace rtype::client {

/** @brief The colors the debris of the ship of each player color go through,
 * player 1 first: white, then its light, main and dark shades, then the
 * smoke. */
using PilotDebrisRamps =
    std::array<std::array<sf::Color, PILOT_DEBRIS_SHADE_COUNT>,
               static_cast<std::size_t>(PLAYER_COLOR_COUNT)>;

}  // namespace rtype::client
