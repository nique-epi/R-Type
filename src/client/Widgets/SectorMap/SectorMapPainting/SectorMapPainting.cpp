#include "SectorMapPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "BoxCorners.hpp"
#include "ColoredPixel.hpp"
#include "CutCornerBox.hpp"
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "LabelSide.hpp"
#include "MapView.hpp"
#include "MiniMap.hpp"
#include "MotionPreference.hpp"
#include "Outline.hpp"
#include "PlanetPose.hpp"
#include "RingedPlanet.hpp"
#include "SectorLayout.hpp"
#include "SectorMapConstants.hpp"
#include "SectorMapPaintingConstants.hpp"
#include "SectorMapSpriteConstants.hpp"
#include "ServerTooltip.hpp"
#include "SpriteFrame.hpp"
#include "StarLook.hpp"
#include "StarMark.hpp"
#include "StarName.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
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

/** @brief Where the lines of a bubble stand, from its top: the first one and
 * from one to the next. */
struct TooltipRows {
  int firstLine{};
  int pitch{};
};

/** @returns The rows of a bubble written at @p metrics: as drawn at the size
 * the fonts are drawn at; above it, under the taller title, one line height
 * and TOOLTIP_LINE_GAP apart. */
TooltipRows tooltipRowsOf(TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return {.firstLine = TOOLTIP_FIRST_LINE, .pitch = TOOLTIP_LINE_PITCH};
  }
  const int lineHeight = lineHeightOf(metrics);
  return {.firstLine = TOOLTIP_TITLE_TOP + lineHeight + TOOLTIP_TITLE_GAP,
          .pitch = lineHeight + TOOLTIP_LINE_GAP};
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
  canvas.write(std::to_string(milliseconds) + std::string{PING_UNIT_SUFFIX},
               {crossing + RING_LABEL_SHIFT,
                SECTOR_MAP_AREA.position.y + SECTOR_MAP_AREA.size.y -
                    (RING_LABEL_LIFT - TEXT_LINE_HEIGHT +
                     lineHeightOf(canvas.textMetrics()))},
               TextStyle{.font = TextFont::Small, .color = BLUE});
}

/** @returns The frames of the planet of a server that looks like @p look:
 * dark and still for another version, ringed in orange when slow, else
 * larger with more pilots. */
std::span<const game::SpriteFrame> planetFramesOf(const StarLook& look) {
  if (look.incompatible) {
    return OTHER_VERSION_PLANET_FRAMES;
  }
  if (look.slow) {
    return SLOW_PLANET_FRAMES;
  }
  if (look.playerCount > CROWDED_SERVER_PILOTS) {
    return LARGE_PLANET_FRAMES;
  }
  return look.playerCount > 0
             ? std::span<const game::SpriteFrame>{MEDIUM_PLANET_FRAMES}
             : SMALL_PLANET_FRAMES;
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

std::size_t planetTurnAfter(engine::Duration elapsed, MotionPreference motion) {
  if (motion == MotionPreference::Reduced) {
    return 0;
  }
  return static_cast<std::size_t>(elapsed / PLANET_TURN_FRAME_TIME) %
         PLANET_TURN_FRAME_COUNT;
}

std::vector<ColoredPixel> sectorPlanetPixels(PlanetPose pose) {
  return ringedPlanetPixels(pose, innerMap());
}

sf::IntRect tooltipAreaOf(const ServerTooltip& tooltip) {
  const int lineCount = static_cast<int>(tooltip.lines.size());
  const TooltipRows rows = tooltipRowsOf(tooltip.metrics);
  return sf::IntRect{
      tooltip.topLeft,
      {tooltip.width, rows.firstLine + (lineCount * rows.pitch) +
                          (TOOLTIP_BOTTOM - TOOLTIP_FIRST_LINE)}};
}

int tooltipWidthFor(const IInterfaceCanvas& canvas,
                    const ServerTooltip& tooltip) {
  if (tooltip.metrics == TextMetrics{}) {
    return tooltip.width;
  }
  int widest = canvas.textWidth(tooltip.title, TextFont::Regular);
  for (const TooltipLine& line : tooltip.lines) {
    widest = std::max(widest, canvas.textWidth(line.text, TextFont::Small));
  }
  return std::max(tooltip.width, widest + (2 * TOOLTIP_PADDING));
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
  const std::span<const game::SpriteFrame> frames = planetFramesOf(star.look);
  const game::SpriteFrame frame = frames[star.turn % frames.size()];
  canvas.drawImage(
      TextureArea{.assetId = SECTOR_MAP_SHEET_ID,
                  .rectangle = rectangleOf(frame)},
      ImagePlacement{.topLeft = star.point - sf::Vector2i{frame.width / 2,
                                                          frame.height / 2}},
      SECTOR_MAP_AREA);
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
  const TooltipRows rows = tooltipRowsOf(tooltip.metrics);
  for (int index = 0; index < lineCount; ++index) {
    const TooltipLine& line = tooltip.lines.at(static_cast<std::size_t>(index));
    canvas.write(
        line.text,
        tooltip.topLeft + sf::Vector2i{TOOLTIP_PADDING,
                                       rows.firstLine + (index * rows.pitch)},
        TextStyle{.font = TextFont::Small, .color = line.color});
  }
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
