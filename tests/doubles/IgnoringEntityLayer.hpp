#pragma once

#include <SFML/System/Vector2.hpp>
#include "IEntityLayer.hpp"

namespace sf {
class RenderTarget;
}  // namespace sf

namespace rtype::engine {
class ComponentRegistry;
}  // namespace rtype::engine

/** @brief An entity layer that ignores every request, for a game screen whose
 * drawing the test does not read. */
class IgnoringEntityLayer : public rtype::client::IEntityLayer {
 public:
  void paint(rtype::engine::ComponentRegistry& components,
             sf::Vector2i offset) override;
  void show(sf::RenderTarget& target) const override;
};
