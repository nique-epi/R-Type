#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include "IPixelSurface.hpp"

namespace rtype::client {

/**
 * @brief Collects plotted pixels as one point each, and draws them in a
 * single call, in the order they were plotted.
 *
 * Meant for a render target whose view maps one unit to one pixel: SFML draws
 * a point one pixel thick whatever the view, so on any other view a point
 * would not cover a whole background pixel.
 */
class PixelBatch final : public IPixelSurface, public sf::Drawable {
 public:
  void plot(sf::Vector2i pixel, sf::Color color) override;

  /** @brief Forgets every point, keeping the memory for the next frame. */
  void clear();

  /** @returns The plotted pixels as points, in the order they were plotted. */
  [[nodiscard]] const sf::VertexArray& points() const;

 private:
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  sf::VertexArray points_{sf::PrimitiveType::Points};
};

}  // namespace rtype::client
