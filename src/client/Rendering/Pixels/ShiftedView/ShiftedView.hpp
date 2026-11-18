#pragma once

#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @returns A copy of @p view through which whatever is drawn lands @p pixels
 * further right and down, one unit of the view being one pixel of a
 * PixelCanvas: the shake of the camera, on whole pixels. */
[[nodiscard]] sf::View shiftedBy(const sf::View& view, sf::Vector2i pixels);

}  // namespace rtype::client
