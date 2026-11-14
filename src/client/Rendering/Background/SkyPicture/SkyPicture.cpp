#include "SkyPicture.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "ClientException.hpp"
#include "EnlargedPicture.hpp"
#include "PixelRounding.hpp"
#include "PlanePlacement.hpp"
#include "ShiftedView.hpp"

namespace rtype::client {

SkyPicture::SkyPicture(std::size_t planeCount)
    : planes_(planeCount), shifts_(planeCount) {
  for (sf::RenderTexture& plane : planes_) {
    if (!plane.resize(PLANE_CANVAS_SIZE)) {
      throw RenderTextureNotCreatedException(PLANE_CANVAS_SIZE.x,
                                             PLANE_CANVAS_SIZE.y);
    }
  }
  if (!flatPicture_.resize(BACKGROUND_SIZE)) {
    throw RenderTextureNotCreatedException(BACKGROUND_SIZE.x,
                                           BACKGROUND_SIZE.y);
  }
  flatPicture_.setSmooth(true);
}

sf::RenderTexture& SkyPicture::paintPlane(std::size_t plane,
                                          PlanePlacement placement) {
  sf::RenderTexture& canvas = planes_.at(plane);
  canvas.clear(sf::Color::Transparent);
  canvas.setView(
      shiftedBy(canvas.getDefaultView(),
                placement.cameraOffset + sf::Vector2i(PLANE_MARGIN, 0)));
  shifts_.at(plane) = placement.shift;
  return canvas;
}

void SkyPicture::leavePlaneEmpty(std::size_t plane) {
  shifts_.at(plane) = std::nullopt;
}

void SkyPicture::show(sf::RenderTarget& target) {
  sf::RenderTexture& picture = enlarged_.matching(target);
  layPlanes(picture, enlarged_.factor());
  enlarged_.layOver(target, sf::BlendNone);
}

const sf::Texture& SkyPicture::flatPicture() {
  sf::Sprite whole(enlarged_.texture());
  const float scale = 1.0F / static_cast<float>(enlarged_.factor());
  whole.setScale({scale, scale});
  flatPicture_.draw(whole, sf::BlendNone);
  flatPicture_.display();
  return flatPicture_.getTexture();
}

void SkyPicture::layPlanes(sf::RenderTexture& picture, unsigned int factor) {
  const auto scale = static_cast<float>(factor);
  picture.clear(SKY_COLOR);
  for (std::size_t index = 0; index < planes_.size(); ++index) {
    const std::optional<float>& shift = shifts_[index];
    if (!shift.has_value()) {
      continue;
    }
    planes_[index].display();
    sf::Sprite plane(planes_[index].getTexture());
    plane.setScale({scale, scale});
    plane.setPosition(
        {static_cast<float>(enlargedPixels(*shift - PLANE_MARGIN, factor)),
         0.0F});
    picture.draw(plane, premultipliedAlphaBlend());
  }
}

}  // namespace rtype::client
