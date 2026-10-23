#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include "Position.hpp"
#include "TimeConstants.hpp"

/** @brief Where the effects of the tests stand, in logical units, and the
 * pixel of the canvas that is: 4 units a pixel. */
constexpr rtype::game::Position ON_PIXEL_100_50{.x = 400.0F, .y = 200.0F};
constexpr sf::Vector2i PIXEL_100_50(100, 50);

/** @brief Ages of a drop of A_PARTICLE_LIFE: a quarter of its life, past two
 * fifths of it, and still waiting 10 ms. */
constexpr rtype::engine::Duration YOUNG = std::chrono::milliseconds(100);
constexpr rtype::engine::Duration PAST_TWO_FIFTHS =
    std::chrono::milliseconds(161);
constexpr rtype::engine::Duration STILL_WAITING = std::chrono::milliseconds(10);

/** @brief The colors the effects of the tests go through. */
constexpr std::array RAMP{sf::Color::White, sf::Color::Yellow, sf::Color::Red,
                          sf::Color::Black};

/** @brief A shockwave of 24 pixels, the radius it opened to at half its life,
 * 7/8 of the way, and an age in the last tenth of A_PARTICLE_LIFE. */
constexpr int RING_RADIUS = 24;
constexpr int RADIUS_AT_HALF_LIFE = 21;
constexpr rtype::engine::Duration LAST_TENTH = std::chrono::milliseconds(360);
