#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include "PointerEvents.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief Where the design lays the court, its banners first. */
constexpr sf::Vector2i LEFT_BANNER(8, 0);
constexpr sf::Vector2i RIGHT_BANNER(420, 0);

/** @brief How long a frame of the banner lasts, as the design says; the time
 * just under it. */
constexpr engine::Duration BANNER_FRAME = std::chrono::milliseconds(50);

/** @brief Where the name of the first row is written: 28 into a list at 92. */
constexpr sf::Vector2i FIRST_NAME(120, 73);

/** @brief The middle of the second tab, of BACK, in pixels of the window. */
constexpr sf::Vector2i ON_THE_SECOND_TAB =
    sf::Vector2i(312, 58) * INTERFACE_PIXEL;
constexpr sf::Vector2i ON_THE_BACK_CHIP =
    sf::Vector2i(360, 232) * INTERFACE_PIXEL;

}  // namespace rtype::client::testing
