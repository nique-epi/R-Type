#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>

namespace rtype::client::testing {

/** @brief A pale cream bulb, lit; the same with each channel one step under
 * the floor a lit bulb passes (red 221, green 186, blue 111 are the first
 * values above it); and the gold of a frame. */
constexpr sf::Color PALE_CREAM_BULB(250, 236, 180);
constexpr sf::Color RED_AT_THE_FLOOR(220, 236, 180);
constexpr sf::Color GREEN_AT_THE_FLOOR(250, 185, 180);
constexpr sf::Color BLUE_AT_THE_FLOOR(250, 236, 110);
constexpr sf::Color FRAME_GOLD(234, 181, 60);

/** @brief The step of the chase lasts 120 ms: a time just short of it, and
 * the time it ends. */
constexpr std::chrono::milliseconds JUST_SHORT_OF_A_STEP{119};
constexpr std::chrono::milliseconds ONE_WHOLE_STEP{120};

/** @brief A beam pointing straight up from the middle of a room, ten pixels
 * long and 0.2 radians wide on each side: the pixel four rows up its axis is
 * in it, the one four columns aside is not. */
constexpr sf::Vector2f BEAM_ORIGIN(100.0F, 100.0F);
constexpr float STRAIGHT_UP = 0.0F;
constexpr float BEAM_LENGTH = 10.0F;
constexpr float BEAM_HALF_SPREAD = 0.2F;
constexpr sf::Color BEAM_COLOR(251, 228, 184, 15);
constexpr sf::Vector2i ON_THE_BEAM_AXIS(100, 95);
constexpr sf::Vector2i BESIDE_THE_BEAM(104, 95);

/** @brief A beam pointing up from the middle of a pixel four rows under the
 * top edge of the interface: it lights rows 0 to 3, and every point of it
 * above row 0 lies outside. */
constexpr sf::Vector2f NEAR_THE_TOP_EDGE(2.5F, 4.0F);

}  // namespace rtype::client::testing
