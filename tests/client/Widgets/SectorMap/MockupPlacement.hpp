#pragma once

#include <SFML/System/Vector2.hpp>
#include "StarBearing.hpp"

namespace rtype::client::testing {

/** @brief Where the drawing kit of the mockup puts a server of a bearing, read
 * by running its place7() on the view of the map zoomed out. */
struct MockupPlacement {
  StarBearing bearing;
  sf::Vector2i pixel;
};

}  // namespace rtype::client::testing
