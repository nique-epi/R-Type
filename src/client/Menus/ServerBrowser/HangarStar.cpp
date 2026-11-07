#include "HangarStar.hpp"
#include <SFML/System/Vector2.hpp>
#include <span>
#include <string>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "ColoredPixel.hpp"
#include "HangarScreenIdConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "LabelSide.hpp"
#include "PixelCircle.hpp"
#include "SectorMapPainting.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ServerTooltip.hpp"
#include "StarName.hpp"
#include "StarNavigation.hpp"
#include "TooltipLine.hpp"
#include "WidgetColors.hpp"

namespace rtype::client {

bool hangarStarAt(sf::Vector2i pixel) {
  return starUnder(std::span(&HANGAR_STAR_POINT, 1), pixel).has_value();
}

void paintHangarStar(IInterfaceCanvas& canvas, bool shown) {
  std::vector<ColoredPixel> pixels;
  for (const sf::Vector2i offset : circleOf(HANGAR_STAR_RADIUS)) {
    pixels.push_back(
        {.position = HANGAR_STAR_POINT + offset, .color = STAR_GOLD});
  }
  pixels.push_back({.position = HANGAR_STAR_POINT, .color = STAR_WHITE});
  canvas.plot(pixels);
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
