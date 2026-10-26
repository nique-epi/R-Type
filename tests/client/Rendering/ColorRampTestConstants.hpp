#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>

namespace rtype::client::testing {

/** @brief A ramp of three colors, red first. */
constexpr sf::Color RED_SHADE(255, 0, 0);
constexpr sf::Color GREEN_SHADE(0, 255, 0);
constexpr sf::Color BLUE_SHADE(0, 0, 255);
constexpr std::array RAMP_OF_THREE{RED_SHADE, GREEN_SHADE, BLUE_SHADE};

/** @brief Shares of a life: half way, and past its end. */
constexpr float HALF_WAY_THROUGH_A_LIFE = 0.5F;
constexpr float PAST_THE_END_OF_A_LIFE = 1.5F;

}  // namespace rtype::client::testing
