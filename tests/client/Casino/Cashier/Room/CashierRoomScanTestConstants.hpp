#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief A picture smaller than the interface, which the cashier cannot
 * read: its vault and its grille would lie outside it. */
constexpr sf::Vector2u CAGE_TOO_SMALL_A_PICTURE(8, 8);

}  // namespace rtype::client::testing
