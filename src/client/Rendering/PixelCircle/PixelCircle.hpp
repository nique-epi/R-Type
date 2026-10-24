#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>

namespace rtype::client {

/** @returns The pixels of a circle of that radius around (0, 0), from the
 * midpoint circle algorithm: one pixel thick, every pixel whole. */
[[nodiscard]] std::vector<sf::Vector2i> circleOf(int radius);

}  // namespace rtype::client
