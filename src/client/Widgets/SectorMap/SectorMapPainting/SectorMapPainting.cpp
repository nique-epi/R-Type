#include "SectorMapPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "BoxCorners.hpp"
#include "ColoredPixel.hpp"
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "LabelSide.hpp"
#include "MapView.hpp"
#include "MiniMap.hpp"
#include "Outline.hpp"
#include "PlanetPose.hpp"
#include "RingedPlanet.hpp"
#include "SectorLayout.hpp"
#include "SectorMapConstants.hpp"
#include "SectorMapPaintingConstants.hpp"
#include "ServerTooltip.hpp"
#include "StarLook.hpp"
#include "StarMark.hpp"
#include "StarName.hpp"
#include "TextStyle.hpp"
#include "TooltipLine.hpp"

namespace rtype::client {

namespace {

/** @returns The map less its frame of one pixel, where the planet, the rings
 * and the stars are clipped, as the mockup clips them. */
sf::IntRect innerMap() {
  return sf::IntRect{SECTOR_MAP_AREA.position + sf::Vector2i{1, 1},
                     SECTOR_MAP_AREA.size - sf::Vector2i{2, 2}};
}

/** @brief Adds the drawn pixels of the dashed segment from @p start to @p end,
 * @p travelled pixels of the line already behind it. */
void addDashes(std::vector<ColoredPixel>& pixels, sf::Vector2f start,
               sf::Vector2f end, float& travelled) {
  const sf::Vector2f along = end - start;
  const float length = along.length();
  const auto steps = static_cast<int>(std::ceil(length / RING_STEP));
  for (int index = 0; index < steps; ++index) {
    const float step = static_cast<float>(index) * RING_STEP;
    if (std::fmod(travelled + step, RING_DASH_PERIOD) < RING_DASH) {
      const sf::Vector2f point = start + (along * (step / length));
      pixels.push_back(
          ColoredPixel{.position = {static_cast<int>(std::floor(point.x)),
                                    static_cast<int>(std::floor(point.y))},
                       .color = DEEP_BLUE});
    }
  }
  travelled += length;
}

/** @brief Writes the label of the ring of @p milliseconds under the map,
 * where it crosses the axis of the planet, when there is room. */
void paintRingLabel(IInterfaceCanvas& canvas, int milliseconds,
                    const MapView& view) {
  const int crossing = static_cast<int>(std::lround(
      view.centerX +
      ((reachOf(0.0) + spreadDistanceOf(milliseconds)) * view.zoom)));
  const int left = SECTOR_MAP_AREA.position.x;
  if (crossing <= left + RING_LABEL_LEFT_MARGIN ||
      crossing >= left + SECTOR_MAP_AREA.size.x - RING_LABEL_ROOM) {
    return;
  }
  canvas.write(
      std::to_string(milliseconds) + std::string{PING_UNIT_SUFFIX},
      {crossing + RING_LABEL_SHIFT,
       SECTOR_MAP_AREA.position.y + SECTOR_MAP_AREA.size.y - RING_LABEL_LIFT},
      TextStyle{.font = TextFont::Small, .color = BLUE});
}

sf::Color starColorOf(const StarLook& look) {
  if (look.incompatible || look.playerCount == 0) {
    return BLUE;
  }
  return look.slow ? WARNING_ORANGE : STAR_WHITE;
}

/** @brief Paints a cross at @p point whose arms reach @p reach pixels from it,
 * one pixel each side of its centre line. */
void paintCross(IInterfaceCanvas& canvas, sf::Vector2i point, int reach,
                sf::Color color) {
  canvas.fill({{point.x - 1, point.y - reach}, {3, (2 * reach) + 1}}, color);
  canvas.fill({{point.x - reach, point.y - 1}, {(2 * reach) + 1, 3}}, color);
}

/** @returns The color of the @p cell (column, row) of the loupe, or nothing
 * where the loupe is clear: the glass lies between the first and last rims of
 * the rows of the glass. */
std::optional<sf::Color> loupeColorOf(sf::Vector2<std::size_t> cell) {
  const std::string_view line = LOUPE_PICTURE.at(cell.y);
  const char symbol = line.at(cell.x);
  if (symbol == LOUPE_RIM) {
    return HUD_CYAN;
  }
  if (symbol == LOUPE_GLINT) {
    return STAR_WHITE;
  }
  if (symbol == LOUPE_HANDLE) {
    return PALE_BLUE;
  }
  const bool insideTheRim =
      cell.x > line.find(LOUPE_RIM) && cell.x < line.rfind(LOUPE_RIM);
  if (cell.y >= LOUPE_GLASS_FIRST_ROW && cell.y <= LOUPE_GLASS_LAST_ROW &&
      insideTheRim) {
    return DEEP_BLUE;
  }
  return std::nullopt;
}

/** @returns @p length of the map, in pixels of @p map. */
int scaledToMiniMap(const MiniMap& map, int length) {
  return static_cast<int>(std::lround(
      static_cast<float>(length) * static_cast<float>(map.area.size.x) /
      static_cast<float>(SECTOR_MAP_AREA.size.x)));
}

/** @returns Where @p point of the map lands on @p map. */
sf::Vector2i onMiniMap(const MiniMap& map, sf::Vector2i point) {
  const sf::Vector2i fromCorner = point - SECTOR_MAP_AREA.position;
  return map.area.position + sf::Vector2i{scaledToMiniMap(map, fromCorner.x),
                                          scaledToMiniMap(map, fromCorner.y)};
}

/** @returns The disc of the planet and a point per server of @p map, inside
 * its frame only. */
std::vector<ColoredPixel> miniMapPixels(const MiniMap& map) {
  const int radius =
      std::max(MINIMAP_SMALLEST_PLANET, scaledToMiniMap(map, map.planetRadius));
  const sf::Vector2i planet = onMiniMap(map, map.planetCenter);
  std::vector<ColoredPixel> pixels;
  for (int row = -radius; row <= radius; ++row) {
    for (int column = -radius; column <= radius; ++column) {
      if ((row * row) + (column * column) <= radius * radius) {
        pixels.push_back(ColoredPixel{
            .position = planet + sf::Vector2i{column, row}, .color = BLUE});
      }
    }
  }
  for (const sf::Vector2i star : map.stars) {
    pixels.push_back(
        ColoredPixel{.position = onMiniMap(map, star), .color = PALE_BLUE});
  }
  const sf::IntRect inside{map.area.position + sf::Vector2i{1, 1},
                           map.area.size - sf::Vector2i{2, 2}};
  std::erase_if(pixels, [inside](const ColoredPixel& pixel) {
    return !inside.contains(pixel.position);
  });
  return pixels;
}

/** @returns The color of @p name: cyan when chosen, blue for a server of
 * another version, pale blue otherwise. */
sf::Color nameColorOf(const StarName& name) {
  if (name.chosen) {
    return HUD_CYAN;
  }
  return name.incompatible ? BLUE : PALE_BLUE;
}

}  // namespace

std::vector<ColoredPixel> sectorPlanetPixels(PlanetPose pose) {
  return ringedPlanetPixels(pose, innerMap());
}

sf::IntRect tooltipAreaOf(const ServerTooltip& tooltip) {
  const int lineCount = static_cast<int>(tooltip.lines.size());
  return sf::IntRect{
      tooltip.topLeft,
      {tooltip.width, TOOLTIP_BOTTOM + (lineCount * TOOLTIP_LINE_PITCH)}};
}

void paintPingRings(IInterfaceCanvas& canvas, const MapView& view) {
  std::vector<ColoredPixel> pixels;
  for (const int milliseconds : PING_RINGS_MILLISECONDS) {
    const std::vector<sf::Vector2f> contour = pingContourOf(milliseconds, view);
    float travelled = 0.0F;
    for (std::size_t index = 1; index < contour.size(); ++index) {
      addDashes(pixels, contour[index - 1], contour[index], travelled);
    }
    paintRingLabel(canvas, milliseconds, view);
  }
  const sf::IntRect inside = innerMap();
  std::erase_if(pixels, [inside](const ColoredPixel& pixel) {
    return !inside.contains(pixel.position);
  });
  canvas.plot(pixels);
}

void paintStar(IInterfaceCanvas& canvas, const StarMark& star) {
  const sf::IntRect shown{
      SECTOR_MAP_AREA.position + sf::Vector2i{STAR_MARGIN, STAR_MARGIN},
      SECTOR_MAP_AREA.size - sf::Vector2i{2 * STAR_MARGIN, 2 * STAR_MARGIN}};
  if (!shown.contains(star.point)) {
    return;
  }
  const sf::Color color = starColorOf(star.look);
  if (star.look.incompatible) {
    const int half = INCOMPATIBLE_STAR_SIZE / 2;
    Outline({star.point - sf::Vector2i{half, half},
             {INCOMPATIBLE_STAR_SIZE, INCOMPATIBLE_STAR_SIZE}},
            Outline::Line::Solid)
        .paint(canvas, color);
  } else if (star.look.playerCount > CROWDED_SERVER_PILOTS) {
    paintCross(canvas, star.point, CROWDED_STAR_CROSS_REACH, color);
  } else if (star.look.playerCount > 0) {
    paintCross(canvas, star.point, STAR_CROSS_REACH, color);
  } else {
    const int half = EMPTY_STAR_SIZE / 2;
    canvas.fill({star.point - sf::Vector2i{half, half},
                 {EMPTY_STAR_SIZE, EMPTY_STAR_SIZE}},
                color);
  }
}

void paintChosenStar(IInterfaceCanvas& canvas, sf::Vector2i point) {
  const int half = STAR_CORNERS_SIZE / 2;
  const sf::Vector2i corner = point - sf::Vector2i{half, half};
  const int far = STAR_CORNERS_SIZE - 1;
  const int stroke = STAR_CORNER_LENGTH;
  for (const sf::Vector2i tip :
       {corner, corner + sf::Vector2i{far, 0}, corner + sf::Vector2i{0, far},
        corner + sf::Vector2i{far, far}}) {
    const int across = tip.x == corner.x ? tip.x : tip.x - stroke + 1;
    const int down = tip.y == corner.y ? tip.y : tip.y - stroke + 1;
    canvas.fill({{across, tip.y}, {stroke, 1}}, HUD_CYAN);
    canvas.fill({{tip.x, down}, {1, stroke}}, HUD_CYAN);
  }
}

void paintStarName(IInterfaceCanvas& canvas, const StarName& name) {
  if (name.side == LabelSide::None) {
    return;
  }
  const bool left = name.side == LabelSide::Left;
  const sf::Color color = nameColorOf(name);
  canvas.write(name.name,
               {name.point.x + (left ? -LABEL_TEXT_GAP : LABEL_TEXT_GAP),
                name.point.y - LABEL_TEXT_RISE},
               TextStyle{.font = TextFont::Small,
                         .color = color,
                         .outlined = true,
                         .alignment = left ? TextAlignment::Right
                                           : TextAlignment::Left});
}

void paintServerTooltip(IInterfaceCanvas& canvas,
                        const ServerTooltip& tooltip) {
  const int lineCount = static_cast<int>(tooltip.lines.size());
  CutCornerBox(tooltipAreaOf(tooltip), {.border = EDGE_BLUE, .inside = INK})
      .paint(canvas);
  canvas.write(
      tooltip.title,
      tooltip.topLeft + sf::Vector2i{TOOLTIP_PADDING, TOOLTIP_TITLE_TOP},
      TextStyle{.font = TextFont::Regular, .color = HUD_CYAN});
  for (int index = 0; index < lineCount; ++index) {
    const TooltipLine& line = tooltip.lines.at(static_cast<std::size_t>(index));
    canvas.write(
        line.text,
        tooltip.topLeft +
            sf::Vector2i{TOOLTIP_PADDING,
                         TOOLTIP_FIRST_LINE + (index * TOOLTIP_LINE_PITCH)},
        TextStyle{.font = TextFont::Small, .color = line.color});
  }
}

void paintZoomLoupe(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                    std::string_view zoom) {
  std::vector<ColoredPixel> pixels;
  for (std::size_t row = 0; row < LOUPE_PICTURE.size(); ++row) {
    const std::string_view line = LOUPE_PICTURE.at(row);
    for (std::size_t column = 0; column < line.size(); ++column) {
      if (const std::optional<sf::Color> color =
              loupeColorOf(sf::Vector2<std::size_t>{column, row})) {
        pixels.push_back(ColoredPixel{
            .position = topLeft + sf::Vector2i{static_cast<int>(column),
                                               static_cast<int>(row)},
            .color = *color});
      }
    }
  }
  canvas.plot(pixels);
  canvas.write(zoom, topLeft + sf::Vector2i{LOUPE_TEXT_SHIFT, LOUPE_TEXT_DROP},
               TextStyle{.font = TextFont::Small,
                         .color = HUD_CYAN,
                         .outlined = true,
                         .alignment = TextAlignment::Center});
}

void paintMiniMap(IInterfaceCanvas& canvas, const MiniMap& map) {
  CutCornerBox(map.area, {.border = DEEP_BLUE, .inside = INK}).paint(canvas);
  canvas.plot(miniMapPixels(map));
  const sf::Vector2i shownSize{
      std::max(MINIMAP_SMALLEST_FRAME, scaledToMiniMap(map, map.shown.size.x)),
      std::max(MINIMAP_SMALLEST_FRAME, scaledToMiniMap(map, map.shown.size.y))};
  Outline({onMiniMap(map, map.shown.position), shownSize}, Outline::Line::Solid)
      .paint(canvas, HUD_CYAN);
}

void paintSectorFrame(IInterfaceCanvas& canvas) {
  paintBoxCorners(canvas, SECTOR_MAP_AREA, EDGE_BLUE);
}

}  // namespace rtype::client
