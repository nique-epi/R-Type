#pragma once

#include <string>
#include "ILayerCanvas.hpp"
#include "ILayerContent.hpp"
#include "LayerFrame.hpp"
#include "MapPlane.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief A layer that shows the maps of an LDtk layer, its tiles or its
 * props, laid end to end from the origin of the layer and coming back to the
 * first after the last (see mapColumns()). A layer without maps shows
 * nothing.
 */
class MapLayer final : public ILayerContent {
 public:
  /** @param maps The maps, as LdtkProjectFile::plane() reads them.
   * @param tileset The asset id of the image their triangles are cut from. */
  MapLayer(MapPlane maps, std::string tileset);

  void advance(engine::Duration elapsed, const LayerFrame& frame) override;
  void paint(ILayerCanvas& canvas, const LayerFrame& frame) const override;
  [[nodiscard]] int leftEdge() const override;

 private:
  MapPlane maps_;
  std::string tileset_;
};

}  // namespace rtype::client
