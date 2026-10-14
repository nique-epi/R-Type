#include "EntityLayer.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include "AssetLibrary.hpp"
#include "ComponentRegistry.hpp"

namespace rtype::client {

EntityLayer::EntityLayer(const AssetLibrary& library)
    : textures_(library), surface_(canvas_.canvas(), textures_) {}

void EntityLayer::paint(engine::ComponentRegistry& components) {
  sf::RenderTexture& canvas = canvas_.canvas();
  canvas.clear(sf::Color::Transparent);
  renderSystem_.render(components, surface_);
  canvas.display();
}

void EntityLayer::show(sf::RenderTarget& target) const { canvas_.show(target); }

}  // namespace rtype::client
