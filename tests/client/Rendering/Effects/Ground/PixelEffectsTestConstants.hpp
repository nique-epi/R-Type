#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include <chrono>
#include <numbers>
#include "Burst.hpp"
#include "Position.hpp"
#include "Ring.hpp"
#include "TimeConstants.hpp"

/** @brief Where the effects of the tests stand, and how long they wait. */
constexpr rtype::game::Position CENTER{.x = 400.0F, .y = 200.0F};
constexpr rtype::engine::Duration WAIT = std::chrono::milliseconds(50);

/** @brief The colors the effects of the tests go through. */
constexpr std::array RAMP{sf::Color::White, sf::Color::Black};

/** @brief The light shade of a pilot, which a ship arriving lays its speed
 * lines in. */
constexpr sf::Color PILOT_LIGHT(127, 230, 255);

/** @brief The angle of the left, in radians, and how far an angle or a speed
 * of a particle may be from its bound. */
constexpr float LEFT = std::numbers::pi_v<float>;
constexpr float BURST_TOLERANCE = 1e-3F;

/** @brief A burst of 8 drops in a cone of one radian, and a shockwave of 24
 * pixels. */
constexpr rtype::client::Burst EIGHT_DROPS{
    .count = 8,
    .slowest = 100.0F,
    .fastest = 200.0F,
    .life = std::chrono::milliseconds(400),
    .lifeSpread = 0.0F,
    .cone = 1.0F,
    .shrinks = true};
constexpr rtype::client::Ring WIDE_RING{.radius = 24,
                                        .life = std::chrono::milliseconds(300)};

/** @brief How far above and under a ship arriving its speed lines lie, from
 * the top, in logical units: 6 and 2 pixels. */
constexpr std::array SPEED_LINE_OFFSETS{-24.0F, -8.0F, 8.0F, 24.0F};
