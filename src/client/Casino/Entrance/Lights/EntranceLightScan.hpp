#pragma once

#include <SFML/Graphics/Image.hpp>
#include "EntranceLights.hpp"

namespace rtype::client {

/** @returns What the front of the casino reads in @p picture, its picture
 * with the doors closed, CASINO_DOORS_CLOSED_ID: nothing when the picture is
 * not as large as the interface. */
[[nodiscard]] EntranceLights entranceLightsOf(const sf::Image& picture);

}  // namespace rtype::client
