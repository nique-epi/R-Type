#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace rtype::client {

/** @brief Adds to @p triangles the two triangles of a rectangle that covers
 * @p place, textured from the rectangle @p texture of its image; a
 * negative width or height of @p texture mirrors the image on that axis. */
void appendQuad(sf::VertexArray& triangles, const sf::FloatRect& place,
                const sf::FloatRect& texture);

}  // namespace rtype::client
