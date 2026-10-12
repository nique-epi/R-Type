#include "PixelBatch.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include "BackgroundConstants.hpp"

namespace rtype::client {

void PixelBatch::plot(sf::Vector2i pixel, sf::Color color) {
  points_.append(sf::Vertex{.position = sf::Vector2f(pixel) + PIXEL_CENTER,
                            .color = color});
}

void PixelBatch::clear() { points_.clear(); }

const sf::VertexArray& PixelBatch::points() const { return points_; }

void PixelBatch::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(points_, states);
}

}  // namespace rtype::client
