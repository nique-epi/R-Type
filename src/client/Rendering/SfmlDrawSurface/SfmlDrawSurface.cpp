#include "SfmlDrawSurface.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <string_view>
#include "ITextureSource.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"

namespace rtype::client {

SfmlDrawSurface::SfmlDrawSurface(sf::RenderTarget& target,
                                 const ITextureSource& textures)
    : target_(target),
      textures_(textures),
      logger_(std::string(LOG_MODULE_NAME)) {}

void SfmlDrawSurface::draw(std::string_view assetId, game::Position center) {
  const sf::Texture* texture = textures_.texture(assetId);
  if (texture == nullptr) {
    if (reportedMissingIds_.emplace(assetId).second) {
      logger_.warn("no texture for asset id ", assetId);
    }
    return;
  }
  sf::Sprite sprite(*texture);
  sprite.setOrigin(sf::Vector2f(texture->getSize()) * HALF);
  sprite.setPosition({center.x, center.y});
  target_.draw(sprite);
}

}  // namespace rtype::client
