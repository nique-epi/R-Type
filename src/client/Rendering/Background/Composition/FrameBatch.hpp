#pragma once

#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace sf {
class Texture;
}  // namespace sf

namespace rtype::client {

/** @brief The frames a layer draws from one image, as triangles drawn in a
 * single call. The image must outlive the batch. */
struct FrameBatch {
  const sf::Texture* image{};
  sf::VertexArray triangles{sf::PrimitiveType::Triangles};
};

}  // namespace rtype::client
