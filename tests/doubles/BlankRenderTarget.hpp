#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

/** @brief Render target nothing is ever drawn on, for screen doubles that only
 * write down that they were asked to draw; it needs no display. */
class BlankRenderTarget : public sf::RenderTarget {
 public:
  [[nodiscard]] sf::Vector2u getSize() const override;
};
