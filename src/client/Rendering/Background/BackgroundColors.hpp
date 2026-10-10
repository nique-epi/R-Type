#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>

namespace rtype::client {

/** @brief Color of the empty sky, before the halo lightens it. */
constexpr sf::Color SKY_COLOR(11, 13, 31);

/**
 * @brief Palette of the stars, shooting stars and tails, from the darkest blue
 * to white, plus the gold of warm stars.
 */
constexpr sf::Color DEEP_BLUE(38, 48, 94);
constexpr sf::Color BLUE(74, 90, 168);
constexpr sf::Color PALE_BLUE(143, 162, 230);
constexpr sf::Color STAR_WHITE(238, 241, 255);
constexpr sf::Color STAR_GOLD(255, 211, 138);

/** @brief Shades of the planet, from its dark side to its lit side. */
constexpr std::array PLANET_SHADES{sf::Color(26, 21, 48), sf::Color(59, 45, 92),
                                   sf::Color(107, 79, 143),
                                   sf::Color(176, 143, 208)};

/**
 * @brief Colors of the half of the ring in front of the planet, and of the
 * half behind it.
 */
constexpr sf::Color PLANET_RING_FRONT_COLOR(201, 167, 122);
constexpr sf::Color PLANET_RING_BACK_COLOR(122, 98, 72);

/** @brief Colors of an asteroid, in shadow and in light. */
constexpr sf::Color ASTEROID_SHADOW_COLOR(94, 80, 70);
constexpr sf::Color ASTEROID_LIGHT_COLOR(156, 134, 112);

/** @brief Colors of a comet tail close to its head. */
constexpr sf::Color COMET_CORE_COLOR(200, 244, 255);
constexpr sf::Color COMET_GLOW_COLOR(111, 182, 214);

/**
 * @brief Tint of the halo: white at 70 % opacity, so the halo adds 70 % of the
 * blurred background over it.
 */
constexpr sf::Color HALO_TINT(255, 255, 255, 179);

}  // namespace rtype::client
