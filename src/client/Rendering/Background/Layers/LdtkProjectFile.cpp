#include "LdtkProjectFile.hpp"
#include <LDtkLoader/DataTypes.hpp>
#include <LDtkLoader/Entity.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Level.hpp>
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/Tile.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "ClientException.hpp"
#include "MapPlane.hpp"
#include "MapTile.hpp"
#include "MapTileVertices.hpp"
#include "PathText.hpp"
#include "TileCornerConstants.hpp"

namespace rtype::client {

namespace {

using TileCorners = std::array<sf::Vector2f, TILE_CORNERS>;

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

/** @returns The corners of a rectangle whose top-left corner is
 * @p topLeft, @p size wide and high, clockwise from the top left. */
TileCorners cornersOf(sf::Vector2f topLeft, sf::Vector2f size) {
  TileCorners corners{};
  corners[TOP_LEFT_CORNER] = topLeft;
  corners[TOP_RIGHT_CORNER] = topLeft + sf::Vector2f{size.x, 0.0F};
  corners[BOTTOM_RIGHT_CORNER] = topLeft + size;
  corners[BOTTOM_LEFT_CORNER] = topLeft + sf::Vector2f{0.0F, size.y};
  return corners;
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
  const TileCorners place = cornersOf(topLeft, size);
  const TileCorners texture = cornersOf(
      {static_cast<float>(source.x), static_cast<float>(source.y)}, size);
  for (const std::size_t corner : TILE_TRIANGLE_CORNERS) {
    triangles.append(sf::Vertex{.position = place.at(corner),
                                .texCoords = texture.at(corner)});
  }
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

/** @returns The plane of the layer @p layer of every map of @p project, each
 * map turned into triangles by @p trianglesOf. It throws what LDtkLoader
 * throws for a map that lacks the layer. */
template <typename TrianglesOf>
MapPlane planeOf(const ldtk::Project& project, std::string_view layer,
                 TrianglesOf trianglesOf) {
  MapPlane plane;
  for (const ldtk::Level& map : project.getWorld().allLevels()) {
    plane.addMap(trianglesOf(map.getLayer(std::string(layer))), map.size.x);
  }
  return plane;
}

}  // namespace

LdtkProjectFile::LdtkProjectFile(const std::filesystem::path& file)
    : project_(std::make_unique<ldtk::Project>()),
      file_(logging::genericText(file)) {
  try {
    project_->loadFromFile(file.string());
  } catch (const std::exception& error) {
    throw BiomeMapException(file_, error.what());
  }
}

LdtkProjectFile::~LdtkProjectFile() = default;

LdtkProjectFile::LdtkProjectFile(LdtkProjectFile&&) noexcept = default;

LdtkProjectFile& LdtkProjectFile::operator=(LdtkProjectFile&&) noexcept =
    default;

MapPlane LdtkProjectFile::tilePlane(std::string_view layer) const {
  try {
    return planeOf(*project_, layer, tileTrianglesOf);
  } catch (const std::exception& error) {
    throw BiomeMapException(file_, error.what());
  }
}

MapPlane LdtkProjectFile::propPlane(std::string_view layer) const {
  try {
    return planeOf(*project_, layer, propTrianglesOf);
  } catch (const std::exception& error) {
    throw BiomeMapException(file_, error.what());
  }
}

}  // namespace rtype::client
