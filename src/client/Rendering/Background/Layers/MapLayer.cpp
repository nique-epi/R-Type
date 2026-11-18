#include "MapLayer.hpp"
#include <string>
#include <utility>
#include "ILayerCanvas.hpp"
#include "LayerFrame.hpp"
#include "MapColumn.hpp"
#include "MapColumns.hpp"
#include "MapPlane.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

MapLayer::MapLayer(MapPlane maps, std::string tileset)
    : maps_(std::move(maps)), tileset_(std::move(tileset)) {}

void MapLayer::advance([[maybe_unused]] engine::Duration elapsed,
                       [[maybe_unused]] const LayerFrame& frame) {}

void MapLayer::paint(ILayerCanvas& canvas, const LayerFrame& frame) const {
  for (const MapColumn& placed : mapColumns(frame.column, maps_.widths())) {
    canvas.drawTiles(tileset_, maps_.tilesOf(placed.map), placed.column);
  }
}

int MapLayer::leftEdge() const {
  return maps_.widths().empty() ? 0 : maps_.firstTileColumnOf(0);
}

}  // namespace rtype::client
