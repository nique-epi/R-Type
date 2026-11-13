#include "MapPlaces.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "LabelSide.hpp"
#include "MapPlace.hpp"
#include "MapPlaceConstants.hpp"
#include "RenderingConstants.hpp"
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

namespace {

/** @brief Paints @p place: its glyph and its name; with @p shown, the
 * corners of a chosen star and its bubble too. */
void paintMapPlace(IInterfaceCanvas& canvas, const MapPlace& place,
                   bool shown) {
  canvas.drawImage(
      TextureArea{.assetId = SECTOR_MAP_SHEET_ID,
                  .rectangle = rectangleOf(place.frame)},
      ImagePlacement{.topLeft =
                         place.point -
                         sf::Vector2i{place.frame.width / HALF_DIVISOR,
                                      place.frame.height / HALF_DIVISOR}},
      SECTOR_MAP_AREA);
  paintStarName(canvas, StarName{.point = place.point,
                                 .name = place.label,
                                 .side = LabelSide::Right,
                                 .chosen = shown,
                                 .incompatible = false});
  if (!shown) {
    return;
  }
  paintChosenStar(canvas, place.point);
  paintServerTooltip(
      canvas,
      ServerTooltip{.topLeft = place.point + place.bubbleOffset,
                    .width = BUBBLE_WIDTH,
                    .title = place.label,
                    .lines = {{.text = place.detail, .color = PAGE_TEXT_COLOR},
                              {.text = std::string(MAP_PLACE_HINT_START) +
                                       place.keyName + MAP_PLACE_HINT_END,
                               .color = STATUS_TEXT_COLOR}}});
}

}  // namespace

std::optional<std::size_t> mapPlaceAt(sf::Vector2i pixel) {
  for (std::size_t place = 0; place < MAP_PLACES.size(); ++place) {
    if (starUnder(std::span(&MAP_PLACES.at(place).point, 1), pixel)) {
      return place;
    }
  }
  return std::nullopt;
}

std::optional<std::size_t> mapPlaceOpenedBy(sf::Keyboard::Key key) {
  for (std::size_t place = 0; place < MAP_PLACES.size(); ++place) {
    if (MAP_PLACES.at(place).key == key) {
      return place;
    }
  }
  return std::nullopt;
}

void paintMapPlaces(IInterfaceCanvas& canvas,
                    std::optional<std::size_t> shown) {
  for (std::size_t place = 0; place < MAP_PLACES.size(); ++place) {
    paintMapPlace(canvas, MAP_PLACES.at(place), shown == place);
  }
}

}  // namespace rtype::client
