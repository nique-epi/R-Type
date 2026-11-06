#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>

namespace rtype::client::testing {

/**
 * @brief A planet of radius 60 whose picture starts at the corner of the
 * grid: node ran drawSystem() of the kit of the mockup on a context that
 * records every pixel, for the frame of the axis in "/", and counted them per
 * color, at rest and turned by 1.5 rad with its grains by 2.0 rad.
 */
constexpr int MOCKUP_RADIUS = 60;
constexpr int MOCKUP_PICTURE_HALF = 134;
constexpr sf::IntRect WHOLE_PICTURE{{0, 0}, {269, 269}};
constexpr std::size_t MOCKUP_PIXELS_AT_REST = 3129;
constexpr std::size_t MOCKUP_DEEP_AT_REST = 546;
constexpr std::size_t MOCKUP_BLUE_AT_REST = 872;
constexpr std::size_t MOCKUP_PALE_AT_REST = 1188;
constexpr std::size_t MOCKUP_CYAN_AT_REST = 523;
constexpr double TURNED_SPIN = 1.5;
constexpr double TURNED_DRIFT = 2.0;
constexpr std::size_t MOCKUP_PIXELS_TURNED = 3095;
constexpr std::size_t MOCKUP_CYAN_TURNED = 484;

/** @brief A pixel the mockup paints pale at rest, and the right half of the
 * picture, as the map shows it. */
constexpr sf::Vector2i MOCKUP_PALE_PIXEL{150, 120};
constexpr sf::IntRect RIGHT_HALF{{134, 0}, {135, 269}};

/** @brief Moments of the turn: one frame, a half turn and a whole turn of the
 * planet, 30 s, and a moment between two frames of 24 a second. */
constexpr std::chrono::milliseconds HALF_A_TURN{15'000};
constexpr std::chrono::milliseconds A_WHOLE_TURN{30'000};
constexpr std::chrono::milliseconds INSIDE_THE_FIRST_FRAME{20};

}  // namespace rtype::client::testing
