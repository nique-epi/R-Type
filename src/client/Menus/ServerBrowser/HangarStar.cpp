#include "HangarStar.hpp"
#include <SFML/System/Vector2.hpp>
#include <span>
#include <string>
#include "EntitySprites.hpp"
#include "HangarScreenIdConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "LabelSide.hpp"
#include "SectorMapConstants.hpp"
#include "SectorMapPainting.hpp"
#include "SectorMapSpriteConstants.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ServerTooltip.hpp"
#include "StarName.hpp"
#include "StarNavigation.hpp"
#include "TextureArea.hpp"
#include "WidgetColors.hpp"

namespace rtype::client {

bool hangarStarAt(sf::Vector2i pixel) {
  return starUnder(std::span(&HANGAR_STAR_POINT, 1), pixel).has_value();
}

void paintHangarStar(IInterfaceCanvas& canvas, bool shown) {
  canvas.drawImage(
      TextureArea{.assetId = SECTOR_MAP_SHEET_ID,
                  .rectangle = rectangleOf(HANGAR_HOUSE_FRAME)},
      ImagePlacement{.topLeft = HANGAR_STAR_POINT -
                                sf::Vector2i{HANGAR_HOUSE_SIZE / 2,
                                             HANGAR_HOUSE_SIZE / 2}},
      SECTOR_MAP_AREA);
  paintStarName(canvas, StarName{.point = HANGAR_STAR_POINT,
                                 .name = HANGAR_LABEL,
                                 .side = LabelSide::Right,
                                 .chosen = shown,
                                 .incompatible = false});
  if (!shown) {
    return;
  }
  paintChosenStar(canvas, HANGAR_STAR_POINT);
  paintServerTooltip(
      canvas,
      ServerTooltip{
          .topLeft = HANGAR_STAR_POINT + BUBBLE_OFFSET,
          .width = BUBBLE_WIDTH,
          .title = HANGAR_LABEL,
          .lines = {{.text = HANGAR_STAR_DETAIL, .color = PAGE_TEXT_COLOR},
                    {.text = std::string(HANGAR_STAR_HINT_START) +
                             HANGAR_KEY_NAME + HANGAR_STAR_HINT_END,
                     .color = STATUS_TEXT_COLOR}}});
}

}  // namespace rtype::client
