#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief Colors read in effects.png: the fire of the explosions and of the
 * impact, after FLASH_COLOR, and of the flame behind a ship. They sit with the
 * ground effects, which paint that flame, and EffectColorConstants.hpp includes
 * them for the other effects. */
constexpr sf::Color FIRE_YELLOW(255, 242, 168);
constexpr sf::Color FIRE_GOLD(255, 197, 49);
constexpr sf::Color FIRE_RED(255, 106, 61);

}  // namespace rtype::client
