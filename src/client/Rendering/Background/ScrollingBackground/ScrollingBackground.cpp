#include "ScrollingBackground.hpp"
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <utility>
#include "AssetFileLookup.hpp"
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "BiomeConstants.hpp"
#include "PlanePlacement.hpp"
#include "SkyLevel.hpp"
#include "StarPlane.hpp"
#include "TextureLookup.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ScrollingBackground::ScrollingBackground(std::uint32_t seed,
                                         TextureLookup textureOf,
                                         const AssetFileLookup& fileOf)
    : starfield_(seed),
      decor_(std::move(textureOf), fileOf),
      sky_(STAR_PLANES.size() + MOST_BIOME_LAYERS) {}

void ScrollingBackground::advance(engine::Duration elapsed) {
  starfield_.advance(elapsed);
  decor_.advance(elapsed);
}

void ScrollingBackground::setSpeedFactor(float factor) {
  starfield_.setSpeedFactor(factor);
}

void ScrollingBackground::setOffset(sf::Vector2i pixels) { offset_ = pixels; }

void ScrollingBackground::showLevel(const SkyLevel& level) {
  decor_.show(level);
}

void ScrollingBackground::draw(sf::RenderTarget& target) {
  for (const StarPlane plane : STAR_PLANES) {
    paintStarPlane(plane);
  }
  for (std::size_t layer = 0; layer < MOST_BIOME_LAYERS; ++layer) {
    paintDecorPlane(layer);
  }
  sky_.show(target);
  drawHalo(target);
}

void ScrollingBackground::paintStarPlane(StarPlane plane) {
  pixels_.clear();
  starfield_.paint(plane, pixels_);
  sky_.paintPlane(static_cast<std::size_t>(plane),
                  PlanePlacement{.cameraOffset = offset_,
                                 .shift = starfield_.shiftOf(plane)})
      .draw(pixels_);
}

void ScrollingBackground::paintDecorPlane(std::size_t layer) {
  const std::size_t index = STAR_PLANES.size() + layer;
  sf::RenderTexture& canvas =
      sky_.paintPlane(index, PlanePlacement{.cameraOffset = offset_,
                                            .shift = decor_.layerShift(layer)});
  if (!decor_.drawLayer(layer, canvas)) {
    sky_.leavePlaneEmpty(index);
  }
}

void ScrollingBackground::drawHalo(sf::RenderTarget& target) {
  if (!halo_.isAvailable()) {
    return;
  }
  sf::Sprite halo(halo_.apply(sky_.flatPicture()));
  halo.setScale({BACKGROUND_PIXEL_SIZE, BACKGROUND_PIXEL_SIZE});
  halo.setColor(HALO_TINT);
  target.draw(halo, sf::BlendAdd);
}

}  // namespace rtype::client
