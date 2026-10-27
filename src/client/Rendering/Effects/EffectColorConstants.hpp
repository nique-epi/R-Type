#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include "BackgroundColorConstants.hpp"
#include "FireColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "SpriteToneColorConstants.hpp"

namespace rtype::client {

/** @brief Colors read in effects.png after the fire (FireColorConstants.hpp):
 * the ember, and the smoke every ramp ends on. */
constexpr sf::Color EMBER(122, 31, 77);
constexpr sf::Color SMOKE(42, 22, 64);

/** @brief Colors of the Bydo, read in entities.png, from its pale shine to
 * its dark purple. */
constexpr sf::Color BYDO_PALE(255, 214, 224);
constexpr sf::Color BYDO_LILAC(217, 179, 255);
constexpr sf::Color BYDO_VIOLET(154, 95, 208);
constexpr sf::Color BYDO_PURPLE(90, 47, 134);

/** @brief Dark shade of the ship of each player color, read from the rows of
 * the entity sheet, like PLAYER_COLORS. */
constexpr sf::Color PLAYER_DARK_BLUE(15, 90, 115);
constexpr sf::Color PLAYER_DARK_PINK(122, 31, 77);
constexpr sf::Color PLAYER_DARK_GREEN(61, 107, 26);
constexpr sf::Color PLAYER_DARK_ORANGE(122, 74, 16);

/**
 * @brief The colors a particle or a shockwave goes through as it ages,
 * hottest first. With REDUCE FLASHES, a ramp loses its first color, but those
 * of the shockwave of a ship, which start in no white.
 *
 * An orange or a red never colors a particle of more than one pixel: two by
 * two, it would read as a missile of an enemy.
 */
constexpr std::array BYDO_RAMP{BYDO_PALE, BYDO_LILAC, BYDO_VIOLET, BYDO_PURPLE,
                               SMOKE};
constexpr std::array SPARK_RAMP{FLASH_COLOR, FIRE_YELLOW, FIRE_GOLD,
                                FIRE_RED,    EMBER,       SMOKE};
constexpr std::array BIG_CATCH_RING_RAMP{STAR_WHITE, PALE_BLUE, BLUE,
                                         DEEP_BLUE};

/** @brief The debris of the ship of each player color, in the order of
 * PLAYER_COLORS, then those of a ship whose pilot is not known. The
 * shockwave of the ship of a pilot goes through the same colors but the
 * first. */
constexpr std::array PILOT_DEBRIS_RAMPS{
    std::array{FLASH_COLOR, PLAYER_LIGHT_BLUE, PLAYER_BLUE, PLAYER_DARK_BLUE,
               SMOKE},
    std::array{FLASH_COLOR, PLAYER_LIGHT_PINK, PLAYER_PINK, PLAYER_DARK_PINK,
               SMOKE},
    std::array{FLASH_COLOR, PLAYER_LIGHT_GREEN, PLAYER_GREEN, PLAYER_DARK_GREEN,
               SMOKE},
    std::array{FLASH_COLOR, PLAYER_LIGHT_ORANGE, PLAYER_ORANGE,
               PLAYER_DARK_ORANGE, SMOKE}};
constexpr std::array UNKNOWN_PILOT_DEBRIS_RAMP{FLASH_COLOR, STAR_WHITE,
                                               PALE_BLUE, BLUE, SMOKE};

/** @brief The shockwave of a ship whose pilot is not known. */
constexpr std::array UNKNOWN_PILOT_RING_RAMP{PALE_BLUE, BLUE, DEEP_BLUE, SMOKE};

/** @brief Light shade given to the speed lines of a ship whose pilot is not
 * known. */
constexpr sf::Color UNKNOWN_PILOT_LIGHT = STAR_WHITE;

}  // namespace rtype::client
