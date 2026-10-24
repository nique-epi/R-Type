#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include <chrono>
#include "Position.hpp"
#include "TimeConstants.hpp"

/** @brief Where the particles of the tests start, and their speed, in logical
 * units a second, to the right. */
constexpr rtype::game::Position START{.x = 400.0F, .y = 200.0F};
constexpr float SPEED = 600.0F;

/** @brief A sixtieth and a thirtieth of a second. */
constexpr rtype::engine::Duration SIXTIETH =
    std::chrono::nanoseconds(16'666'667);
constexpr rtype::engine::Duration THIRTIETH =
    std::chrono::nanoseconds(33'333'333);

/** @brief The colors the particles of the tests go through. */
constexpr std::array RAMP{sf::Color::White};

/** @brief The share of its speed a particle keeps over a sixtieth of a
 * second, and how far a speed may be from what it must be. */
constexpr float SPEED_KEPT_PER_SIXTIETH = 0.9F;
constexpr float SPEED_TOLERANCE = 1e-2F;
