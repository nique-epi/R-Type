#include "MapLayerFixtures.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <string>
#include <utility>
#include <vector>
#include "LayerFrame.hpp"
#include "MapLayer.hpp"
#include "MapLayerTestConstants.hpp"
#include "MapPlane.hpp"
#include "MapTileVertices.hpp"
#include "MapTileVerticesTestConstants.hpp"
#include "RecordedTiles.hpp"
#include "RecordingLayerCanvas.hpp"

using rtype::client::LayerFrame;
using rtype::client::MapLayer;
using rtype::client::MapPlane;
using rtype::client::tileVertices;

MapLayer layerOfTwoMaps() {
  MapPlane maps;
  maps.addMap(tileVertices(ONE_TILE, TILE_SIZE), FIRST_MAP_WIDTH);
  maps.addMap(sf::VertexArray{sf::PrimitiveType::Triangles}, SECOND_MAP_WIDTH);
  return MapLayer{std::move(maps), std::string{WALL_TILESET}};
}

MapLayer layerWithoutMaps() {
  return MapLayer{MapPlane{}, std::string{WALL_TILESET}};
}

std::vector<RecordedTiles> tilesPaintedBy(const MapLayer& layer,
                                          const LayerFrame& frame) {
  RecordingLayerCanvas canvas;
  layer.paint(canvas, frame);
  return canvas.tiles();
}
