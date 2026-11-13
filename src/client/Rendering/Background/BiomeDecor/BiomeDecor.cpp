#include "BiomeDecor.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <chrono>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "AssetFileLookup.hpp"
#include "BackgroundConstants.hpp"
#include "Biome.hpp"
#include "BiomeConstants.hpp"
#include "BiomeLayer.hpp"
#include "BiomeMap.hpp"
#include "ClientException.hpp"
#include "SkyLevel.hpp"
#include "TextureLookup.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

std::optional<std::size_t> biomeIndexOf(std::string_view name) {
  for (std::size_t index = 0; index < BIOMES.size(); ++index) {
    if (BIOMES.at(index).name == name) {
      return index;
    }
  }
  return std::nullopt;
}

BiomeDecor::BiomeDecor(TextureLookup textureOf, const AssetFileLookup& fileOf)
    : textureOf_(std::move(textureOf)),
      logger_(std::string(BACKGROUND_LOGGER_NAME)) {
  for (const Biome& biome : BIOMES) {
    maps_.push_back(readMaps(biome, fileOf));
  }
}

void BiomeDecor::show(const SkyLevel& level) { level_ = level; }

void BiomeDecor::advance(engine::Duration elapsed) {
  if (elapsed <= engine::Duration::zero()) {
    return;
  }
  level_.scrollPosition +=
      level_.scrollSpeed * std::chrono::duration<float>(elapsed).count();
}

void BiomeDecor::draw(sf::RenderTarget& canvas) const {
  const std::optional<std::size_t> biome = biomeIndexOf(level_.biome);
  if (!biome.has_value()) {
    return;
  }
  const std::optional<BiomeMap>& map = maps_.at(*biome);
  if (!map.has_value()) {
    return;
  }
  const std::span<const BiomeLayer> layers = BIOMES.at(*biome).layers;
  for (std::size_t index = 0; index < layers.size(); ++index) {
    const sf::Texture* const tileset = textureOf_(layers[index].tileset);
    if (tileset != nullptr) {
      map->plane(index).draw(canvas, *tileset, enteredPixels(layers[index]));
    }
  }
}

float BiomeDecor::scrollPosition() const { return level_.scrollPosition; }

bool BiomeDecor::hasMapsOf(std::string_view name) const {
  const std::optional<std::size_t> biome = biomeIndexOf(name);
  return biome.has_value() && maps_.at(*biome).has_value();
}

std::optional<BiomeMap> BiomeDecor::readMaps(
    const Biome& biome, const AssetFileLookup& fileOf) const {
  try {
    return BiomeMap(fileOf(biome.project), biome.layers);
  } catch (const BiomeMapException& error) {
    logger_.error("the biome ", biome.name, " shows no decor: ", error.what());
    return std::nullopt;
  }
}

float BiomeDecor::enteredPixels(const BiomeLayer& layer) const {
  return (level_.scrollPosition - level_.biomeSince) * layer.scrollFactor /
         BACKGROUND_PIXEL_SIZE;
}

}  // namespace rtype::client
