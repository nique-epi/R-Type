#include "BiomeDecor.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
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
  const auto* const found = std::ranges::find(BIOMES, name, &Biome::name);
  return found == BIOMES.end() ? std::span<const BiomeLayer>{} : found->layers;
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

BiomeDecor::BiomeDecor(TextureLookup textureOf)
    : textureOf_(std::move(textureOf)) {}

void BiomeDecor::show(const SkyLevel& level) { level_ = level; }

void BiomeDecor::advance(engine::Duration elapsed) {
  if (elapsed <= engine::Duration::zero()) {
    return;
  }
  level_.scrollPosition +=
      level_.scrollSpeed * std::chrono::duration<float>(elapsed).count();
}

void BiomeDecor::draw(sf::RenderTarget& canvas) const {
  for (const BiomeLayer& layer : layersOfBiome(level_.biome)) {
    const sf::Texture* const picture = textureOf_(layer.image);
    if (picture == nullptr) {
      continue;
    }
    sf::Sprite copy(*picture);
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

}  // namespace rtype::client
