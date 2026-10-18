#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief Color a flashing sprite is painted in, keeping its transparency: the
 * white of the explosions, the brightest color of the sheets. */
constexpr sf::Color FLASH_COLOR(255, 251, 232);

/**
 * @brief Color a shaded sprite is multiplied by: each channel of a pixel is
 * scaled by the channel of the tint over 255, so the sprite only darkens,
 * toward the ember red of the explosions. Drawn for a hit when the player
 * reduced flashes.
 */
constexpr sf::Color SHADE_TINT(200, 130, 150);

}  // namespace rtype::client
