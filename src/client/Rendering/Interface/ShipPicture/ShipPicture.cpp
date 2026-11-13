#include "ShipPicture.hpp"
#include <cstddef>
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "PilotHull.hpp"
#include "PilotShipPicture.hpp"
#include "PlayerShading.hpp"
#include "ShipHulls.hpp"
#include "ShipPlacement.hpp"
#include "ShipSheetFrame.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

void paintPilotShip(IInterfaceCanvas& canvas, const PilotShipPicture& picture,
                    ShipPlacement ship) {
  const ShipSheetFrame shown = pilotShipFrameOf(
      PilotHull{.hull = picture.hull, .color = picture.color}, picture.frame);
  canvas.drawImage(
      TextureArea{.assetId = shown.sheet,
                  .rectangle = rectangleOf(shown.frame)},
      ImagePlacement{
          .topLeft = ship.topLeft,
          .scale = ship.scale,
          .recolor = shadingOf(picture.palette,
                               static_cast<std::size_t>(picture.color))
                         .recolor},
      ship.visibleArea);
}

}  // namespace rtype::client
