#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief A picture smaller than the interface, which the hall cannot read:
 * its lights would lie outside it. */
constexpr sf::Vector2u TOO_SMALL_A_PICTURE(8, 8);

/** @brief The fewest spectators the crowd of the hall is cut into: a crowd
 * read as one block would hop as one. */
constexpr std::size_t FEWEST_SPECTATORS = 10;

}  // namespace rtype::client::testing
