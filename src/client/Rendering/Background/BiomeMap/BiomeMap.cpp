#include "BiomeMap.hpp"
#include <LDtkLoader/DataTypes.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Level.hpp>
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/Tile.hpp>
#include <LDtkLoader/World.hpp>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <span>
#include <string>
#include <vector>
#include "BiomeLayer.hpp"
#include "ClientException.hpp"
#include "MapPlane.hpp"
#include "MapTile.hpp"
#include "MapTileVertices.hpp"
#include "PathText.hpp"

namespace rtype::client {

namespace {

MapTile mapTileOf(const ldtk::Tile& tile) {
  const ldtk::IntPoint position = tile.getPosition();
  const ldtk::IntRect source = tile.getTextureRect();
  return MapTile{.position = {position.x, position.y},
                 .source = {source.x, source.y},
                 .flipX = tile.flipX,
                 .flipY = tile.flipY};
}

std::vector<MapTile> tilesOfLayer(const ldtk::Layer& layer) {
  std::vector<MapTile> tiles;
  tiles.reserve(layer.allTiles().size());
  for (const ldtk::Tile& tile : layer.allTiles()) {
    tiles.push_back(mapTileOf(tile));
  }
  return tiles;
}

}  // namespace

BiomeMap::BiomeMap(const std::filesystem::path& project,
                   std::span<const BiomeLayer> layers)
    : planes_(layers.size()) {
  try {
    readProject(project, layers);
  } catch (const std::exception& error) {
    throw BiomeMapException(logging::genericText(project), error.what());
  }
}

const MapPlane& BiomeMap::plane(std::size_t layer) const {
  return planes_.at(layer);
}

void BiomeMap::readProject(const std::filesystem::path& project,
                           std::span<const BiomeLayer> layers) {
  ldtk::Project ldtkProject;
  ldtkProject.loadFromFile(project.string());
  for (const ldtk::Level& map : ldtkProject.getWorld().allLevels()) {
    for (std::size_t index = 0; index < layers.size(); ++index) {
      const ldtk::Layer& layer = map.getLayer(std::string(layers[index].layer));
      planes_[index].addMap(
          tileVertices(tilesOfLayer(layer), layer.getCellSize()), map.size.x);
    }
  }
}

}  // namespace rtype::client
