#include "SmoothImageLayer.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <string_view>
#include "BackgroundConstants.hpp"
#include "InterfaceConstants.hpp"
#include "SmoothImage.hpp"
#include "SmoothImageSpot.hpp"

namespace rtype::client {

SmoothImageLayer::SmoothImageLayer()
    : logger_(std::string(INTERFACE_LOGGER_NAME)) {}

void SmoothImageLayer::clear() { laid_.clear(); }

void SmoothImageLayer::lay(std::string_view assetId, const sf::Texture& texture,
                           SmoothImageSpot spot) {
  laid_.push_back({.texture = &copyOf(assetId, texture),
                   .area = spot.area,
                   .tint = spot.tint});
}

void SmoothImageLayer::display() { shown_ = laid_; }

void SmoothImageLayer::show(sf::RenderTarget& target) const {
  for (const SmoothImage& image : shown_) {
    const sf::Vector2f textureSize(image.texture->getSize());
    const sf::Vector2f areaSize =
        sf::Vector2f(image.area.size) * BACKGROUND_PIXEL_SIZE;
    sf::Sprite sprite(*image.texture);
    sprite.setPosition(sf::Vector2f(image.area.position) *
                       BACKGROUND_PIXEL_SIZE);
    sprite.setScale({areaSize.x / textureSize.x, areaSize.y / textureSize.y});
    sprite.setColor(image.tint);
    target.draw(sprite);
  }
}

const sf::Texture& SmoothImageLayer::copyOf(std::string_view assetId,
                                            const sf::Texture& texture) {
  const auto known = copies_.find(assetId);
  if (known != copies_.end()) {
    return known->second;
  }
  sf::Texture& copy =
      copies_.emplace(std::string(assetId), texture).first->second;
  copy.setSmooth(true);
  if (!copy.generateMipmap()) {
    logger_.warn("no mipmaps for asset id ", assetId, ", smoothed without");
  }
  return copy;
}

}  // namespace rtype::client
