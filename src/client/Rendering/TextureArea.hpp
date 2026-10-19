#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <string_view>

namespace rtype::client {

/** @brief A rectangle of a loaded texture: what IInterfaceCanvas::drawImage()
 * copies. */
struct TextureArea {
  /** @brief Id of the texture; an id with no loaded texture paints nothing. */
  std::string_view assetId;
  /** @brief The rectangle, in pixels of the texture. */
  sf::IntRect rectangle;
};

}  // namespace rtype::client
