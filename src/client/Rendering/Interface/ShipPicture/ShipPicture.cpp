#include "ShipPicture.hpp"
#include <cstddef>
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "PilotShipPicture.hpp"
#include "PlayerShading.hpp"
#include "ShipPlacement.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

void paintPilotShip(IInterfaceCanvas& canvas, const PilotShipPicture& picture,
                    ShipPlacement ship) {
  canvas.drawImage(
      TextureArea{.assetId = ENTITY_SHEET_ID,
                  .rectangle = rectangleOf(
                      inPlayerColorRow(picture.frame, picture.color))},
      ImagePlacement{
          .topLeft = ship.topLeft,
          .scale = ship.scale,
          .recolor = shadingOf(picture.palette,
                               static_cast<std::size_t>(picture.color))
                         .recolor},
      ship.visibleArea);
}

}  // namespace rtype::client
