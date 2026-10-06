#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/**
 * @brief Color filling the playfield, so it stands out from the black bars.
 *
 * Provisional: it stands in for the scrolling background until that one
 * exists.
 */
constexpr sf::Color PLAYFIELD_BACKGROUND_COLOR(10, 12, 40);

}  // namespace rtype::client
