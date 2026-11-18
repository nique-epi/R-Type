#include "LdtkProjectFile.hpp"
#include <LDtkLoader/DataTypes.hpp>
#include <LDtkLoader/Entity.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Level.hpp>
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/Tile.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "ClientException.hpp"
#include "MapLayerKind.hpp"
#include "MapPlane.hpp"
#include "MapTile.hpp"
#include "MapTileVertices.hpp"
#include "PathText.hpp"
#include "TexturedQuad.hpp"

namespace rtype::client {

namespace {

/** @returns The tile @p tile of a tile layer, as tileVertices() takes it. */
MapTile mapTileOf(const ldtk::Tile& tile) {
  const ldtk::IntPoint position = tile.getPosition();
  const ldtk::IntRect source = tile.getTextureRect();
  return MapTile{.position = {position.x, position.y},
                 .source = {source.x, source.y},
                 .flipX = tile.flipX,
                 .flipY = tile.flipY};
}

/** @returns The triangles of the tiles of @p layer. */
sf::VertexArray tileTrianglesOf(const ldtk::Layer& layer) {
  std::vector<MapTile> tiles;
  tiles.reserve(layer.allTiles().size());
  for (const ldtk::Tile& tile : layer.allTiles()) {
    tiles.push_back(mapTileOf(tile));
  }
  return tileVertices(tiles, layer.getCellSize());
}

/** @brief Adds to @p triangles the two triangles of the prop @p entity: the
 * rectangle its tile names, its pivot on the position of the entity. */
void addProp(const ldtk::Entity& entity, sf::VertexArray& triangles) {
  const ldtk::IntRect source = entity.getTextureRect();
  const sf::Vector2f size(static_cast<float>(source.width),
                          static_cast<float>(source.height));
  const sf::Vector2f topLeft(static_cast<float>(entity.getPosition().x) -
                                 (entity.getPivot().x * size.x),
                             static_cast<float>(entity.getPosition().y) -
                                 (entity.getPivot().y * size.y));
  appendQuad(
      triangles, sf::FloatRect{topLeft, size},
      sf::FloatRect{
          {static_cast<float>(source.x), static_cast<float>(source.y)}, size});
}

/** @returns The triangles of the props of @p layer. */
sf::VertexArray propTrianglesOf(const ldtk::Layer& layer) {
  sf::VertexArray triangles(sf::PrimitiveType::Triangles);
  for (const ldtk::Entity& entity : layer.allEntities()) {
    if (entity.hasSprite()) {
      addProp(entity, triangles);
    }
  }
  return triangles;
}

/** @returns The triangles of @p layer, which holds what @p kind says. */
sf::VertexArray trianglesOf(const ldtk::Layer& layer, MapLayerKind kind) {
  return kind == MapLayerKind::Props ? propTrianglesOf(layer)
                                     : tileTrianglesOf(layer);
}

}  // namespace

LdtkProjectFile::LdtkProjectFile(const std::filesystem::path& file)
    : project_(std::make_unique<ldtk::Project>()),
      file_(logging::genericText(file)) {
  try {
    project_->loadFromFile(file.string());
  } catch (const std::exception& error) {
    throw LdtkProjectException(file_, error.what());
  }
}

LdtkProjectFile::~LdtkProjectFile() = default;

LdtkProjectFile::LdtkProjectFile(LdtkProjectFile&&) noexcept = default;

LdtkProjectFile& LdtkProjectFile::operator=(LdtkProjectFile&&) noexcept =
    default;

MapPlane LdtkProjectFile::plane(std::string_view layer,
                                MapLayerKind kind) const {
  try {
    MapPlane maps;
    for (const ldtk::Level& map : project_->getWorld().allLevels()) {
      maps.addMap(trianglesOf(map.getLayer(std::string(layer)), kind),
                  map.size.x);
    }
    return maps;
  } catch (const std::exception& error) {
    throw LdtkProjectException(file_, error.what());
  }
}

}  // namespace rtype::client
