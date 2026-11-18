#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>

namespace rtype::client {

/** @brief A layer that shows one image: the asset id of its file, and where
 * its top-left corner lies in the layer, in background pixels. */
struct ImageContent {
  std::string file;
  sf::Vector2f topLeft;
};

}  // namespace rtype::client
