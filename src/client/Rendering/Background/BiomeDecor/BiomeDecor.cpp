#include "BiomeDecor.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <span>
#include <string_view>
#include <utility>
#include <vector>
#include "BackgroundConstants.hpp"
#include "Biome.hpp"
#include "BiomeConstants.hpp"
#include "BiomeLayer.hpp"
#include "SkyLevel.hpp"
#include "TextureLookup.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

std::span<const BiomeLayer> layersOfBiome(std::string_view name) {
  for (const Biome& biome : BIOMES) {
    if (biome.name == name) {
      return biome.layers;
    }
  }
  return {};
}

std::vector<int> decorTileColumns(float scrolledPixels, int tileWidth) {
  std::vector<int> columns;
  if (tileWidth <= 0) {
    return columns;
  }
  const auto scrolledIntoTile = static_cast<int>(
      std::floor(std::fmod(scrolledPixels, static_cast<float>(tileWidth))));
  for (int column = -scrolledIntoTile;
       column < static_cast<int>(BACKGROUND_WIDTH); column += tileWidth) {
    columns.push_back(column);
  }
  return columns;
}

float arrivingShare(engine::Duration fadeLeft) {
  const engine::Duration left =
      std::clamp(fadeLeft, engine::Duration::zero(), BIOME_FADE_DURATION);
  return 1.0F - (std::chrono::duration<float>(left) /
                 std::chrono::duration<float>(BIOME_FADE_DURATION));
}

BiomeDecor::BiomeDecor(TextureLookup textureOf)
    : textureOf_(std::move(textureOf)) {}

void BiomeDecor::show(const SkyLevel& level) {
  if (level.biome != level_.biome) {
    leavingBiome_ = level_.biome;
    fadeLeft_ = BIOME_FADE_DURATION;
  }
  level_ = level;
}

void BiomeDecor::advance(engine::Duration elapsed) {
  if (elapsed <= engine::Duration::zero()) {
    return;
  }
  level_.scrollPosition +=
      level_.scrollSpeed * std::chrono::duration<float>(elapsed).count();
  fadeLeft_ = std::max(engine::Duration::zero(), fadeLeft_ - elapsed);
}

void BiomeDecor::draw(sf::RenderTarget& canvas) const {
  const float shown = shownShare();
  drawBiome(canvas, leavingBiome_, 1.0F - shown);
  drawBiome(canvas, level_.biome, shown);
}

void BiomeDecor::drawBiome(sf::RenderTarget& canvas, std::string_view biome,
                           float share) const {
  if (share <= 0.0F) {
    return;
  }
  sf::Color tint = sf::Color::White;
  tint.a = static_cast<std::uint8_t>(std::lround(
      share * static_cast<float>(std::numeric_limits<std::uint8_t>::max())));
  for (const BiomeLayer& layer : layersOfBiome(biome)) {
    const sf::Texture* const picture = textureOf_(layer.image);
    if (picture == nullptr) {
      continue;
    }
    sf::Sprite copy(*picture);
    copy.setColor(tint);
    const float scrolledPixels =
        level_.scrollPosition * layer.scrollFactor / BACKGROUND_PIXEL_SIZE;
    for (const int column : decorTileColumns(
             scrolledPixels, static_cast<int>(picture->getSize().x))) {
      copy.setPosition({static_cast<float>(column), 0.0F});
      canvas.draw(copy);
    }
  }
}

float BiomeDecor::scrollPosition() const { return level_.scrollPosition; }

float BiomeDecor::shownShare() const { return arrivingShare(fadeLeft_); }

}  // namespace rtype::client
