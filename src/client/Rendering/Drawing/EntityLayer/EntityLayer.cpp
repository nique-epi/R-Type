#include "EntityLayer.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include "AssetLibrary.hpp"
#include "BackgroundConstants.hpp"
#include "ComponentRegistry.hpp"
#include "GroundEffects.hpp"
#include "ShiftedView.hpp"

namespace rtype::client {

EntityLayer::EntityLayer(const AssetLibrary& library)
    : textures_(library),
      writer_(library),
      canvas_(CRISP_TEXT_DENSITY),
      surface_(canvas_.canvas(), textures_, writer_) {}

void EntityLayer::paint(engine::ComponentRegistry& components,
                        sf::Vector2i offset) {
  sf::RenderTexture& canvas = canvas_.canvas();
  canvas.clear(sf::Color::Transparent);
  canvas.setView(shiftedBy(canvas_.pixelView(), offset));
  groundPixels_.clear();
  paintGroundEffects(components, groundPixels_);
  canvas.draw(groundPixels_);
  renderSystem_.render(components, surface_);
  canvas.setView(canvas_.pixelView());
  canvas.display();
}

void EntityLayer::show(sf::RenderTarget& target) const { canvas_.show(target); }

}  // namespace rtype::client
