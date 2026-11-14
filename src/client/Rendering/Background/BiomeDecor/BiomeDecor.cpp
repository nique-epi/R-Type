#include "BiomeDecor.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <chrono>
#include <cmath>
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
#include "MapPlane.hpp"
#include "PixelRounding.hpp"
#include "SeamCover.hpp"
#include "SkyLevel.hpp"
#include "TextureLookup.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p layer is the nearest of @p layers: the last one, which
 * slides in from the right edge of the sky and keeps its grid. */
bool isNearest(std::size_t layer, std::span<const BiomeLayer> layers) {
  return layer + 1 == layers.size();
}

/** @returns How many pixels of the background @p layer moved by since the
 * biome of @p level began. */
float enteredPixels(const SkyLevel& level, const BiomeLayer& layer) {
  return (level.scrollPosition - level.biomeSince) * layer.scrollFactor /
         BACKGROUND_PIXEL_SIZE;
}

/** @returns The column of the sky where the first map of the plane @p layer
 * starts: the nearest one starts at the right edge of the sky when the biome
 * begins, the others at its left edge, so they cover it whole. */
int firstColumnOf(const SkyLevel& level, std::span<const BiomeLayer> layers,
                  std::size_t layer) {
  const int start =
      isNearest(layer, layers) ? static_cast<int>(BACKGROUND_WIDTH) : 0;
  return start -
         static_cast<int>(std::floor(enteredPixels(level, layers[layer])));
}

}  // namespace

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

bool BiomeDecor::drawLayer(std::size_t layer, sf::RenderTarget& canvas) const {
  const std::optional<std::size_t> biome = biomeIndexOf(level_.biome);
  if (!biome.has_value() || layer >= BIOMES.at(*biome).layers.size()) {
    return false;
  }
  const std::optional<BiomeMap>& maps = maps_.at(*biome);
  const std::span<const BiomeLayer> layers = BIOMES.at(*biome).layers;
  const sf::Texture* const tileset = textureOf_(layers[layer].tileset);
  const std::optional<int> seam = seamColumn();
  const bool behind = !isNearest(layer, layers) && seam.has_value();
  if (!maps.has_value() || tileset == nullptr ||
      (behind && hidesTheWholeCanvas(canvas, *seam))) {
    return false;
  }
  maps->plane(layer).draw(canvas, *tileset,
                          firstColumnOf(level_, layers, layer));
  if (behind) {
    seamCover_.cover(canvas, *seam);
  }
  return true;
}

float BiomeDecor::layerShift(std::size_t layer) const {
  const std::optional<std::size_t> biome = biomeIndexOf(level_.biome);
  if (!biome.has_value()) {
    return 0.0F;
  }
  const std::span<const BiomeLayer> layers = BIOMES.at(*biome).layers;
  if (layer >= layers.size() || isNearest(layer, layers)) {
    return 0.0F;
  }
  return -shiftPastPixel(enteredPixels(level_, layers[layer]));
}

std::optional<int> BiomeDecor::seamColumn() const {
  const std::optional<std::size_t> biome = biomeIndexOf(level_.biome);
  if (!biome.has_value() || BIOMES.at(*biome).layers.empty()) {
    return std::nullopt;
  }
  const std::optional<BiomeMap>& maps = maps_.at(*biome);
  const std::span<const BiomeLayer> layers = BIOMES.at(*biome).layers;
  const std::size_t nearest = layers.size() - 1;
  if (!maps.has_value() || maps->plane(nearest).widths().empty()) {
    return std::nullopt;
  }
  return firstColumnOf(level_, layers, nearest) +
         maps->plane(nearest).firstTileColumnOf(0);
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

}  // namespace rtype::client
