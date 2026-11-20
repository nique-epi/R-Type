#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief A picture smaller than the interface, which the front cannot read:
 * its lights would lie outside it. */
constexpr sf::Vector2u ENTRANCE_TOO_SMALL_A_PICTURE(8, 8);

/** @brief The steps of one turn of the chase, three phases: every bulb is out
 * at one of them. */
constexpr int A_STEP_OF_THE_CHASE = 3;

}  // namespace rtype::client::testing
