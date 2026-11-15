#include "CasinoPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "ButlerLine.hpp"
#include "CasinoBalances.hpp"
#include "CasinoPaintingConstants.hpp"
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "Kopecks.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Paints the four edges of @p area, one pixel wide, in @p color. */
void paintEdges(IInterfaceCanvas& canvas, sf::IntRect area, sf::Color color) {
  const sf::Vector2i end = area.position + area.size - sf::Vector2i(1, 1);
  canvas.fill({area.position, {area.size.x, 1}}, color);
  canvas.fill({{area.position.x, end.y}, {area.size.x, 1}}, color);
  canvas.fill({area.position, {1, area.size.y}}, color);
  canvas.fill({{end.x, area.position.y}, {1, area.size.y}}, color);
}

/** @brief The corners of a gaming plaque: at each corner of an area, a
 * stroke of that many pixels along each side, in one color. */
struct PlaqueCorners {
  sf::IntRect area;
  int length{};
  sf::Color color;
};

/** @brief Paints @p corners: a pixel at each corner of their area, and, with
 * a length above one, that many pixels along each side from it. */
void paintCorners(IInterfaceCanvas& canvas, const PlaqueCorners& corners) {
  const sf::IntRect area = corners.area;
  const int length = corners.length;
  const sf::Vector2i end = area.position + area.size - sf::Vector2i(1, 1);
  for (const sf::Vector2i corner :
       {area.position, sf::Vector2i(end.x, area.position.y),
        sf::Vector2i(area.position.x, end.y), end}) {
    const int left = corner.x == end.x ? corner.x - length + 1 : corner.x;
    const int top = corner.y == end.y ? corner.y - length + 1 : corner.y;
    canvas.fill({{left, corner.y}, {length, 1}}, corners.color);
    canvas.fill({{corner.x, top}, {1, length}}, corners.color);
  }
}

/** @returns @p area, @p inset pixels smaller on every side. */
sf::IntRect insetOf(sf::IntRect area, int inset) {
  const sf::Vector2i margin(inset, inset);
  return {area.position + margin, area.size - (margin * SIDES_PER_AXIS)};
}

}  // namespace

void paintGoldPanel(IInterfaceCanvas& canvas, sf::IntRect area) {
  canvas.fill(area, CASINO_PANEL_INSIDE_COLOR);
  paintEdges(canvas, area, CASINO_GOLD);
  const sf::IntRect inner = insetOf(area, CASINO_PANEL_INNER_EDGE);
  paintEdges(canvas, inner, CASINO_DARK_GOLD);
  paintCorners(canvas, PlaqueCorners{.area = area,
                                     .length = CASINO_PANEL_OUTER_CORNER_LENGTH,
                                     .color = CASINO_TAG_GOLD});
  paintCorners(canvas, PlaqueCorners{.area = inner,
                                     .length = CASINO_PANEL_CORNER_LENGTH,
                                     .color = CASINO_LIGHT_GOLD});
}

void paintNameTag(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                  std::string_view name) {
  const int width = canvas.textWidth(name, CASINO_TAG_STYLE.font) +
                    (CASINO_TAG_PADDING.x * SIDES_PER_AXIS);
  const sf::IntRect tag(topLeft, {width, CASINO_TAG_HEIGHT});
  canvas.fill(tag, CASINO_PANEL_INSIDE_COLOR);
  paintEdges(canvas, tag, CASINO_GOLD);
  canvas.write(name, topLeft + CASINO_TAG_PADDING, CASINO_TAG_STYLE);
}

void paintCasinoBalances(IInterfaceCanvas& canvas, CasinoBalances balances) {
  const sf::IntRect area = CASINO_BALANCES_AREA;
  CutCornerBox(area, {.border = CASINO_BALANCES_BORDER_COLOR,
                      .inside = CASINO_PANEL_INSIDE_COLOR})
      .paint(canvas);
  const int right = area.position.x + area.size.x - CASINO_BALANCES_PADDING;
  const int top = area.position.y;
  canvas.write(kopecksText(balances.kopecks), {right, top + CASINO_KOPECKS_TOP},
               CASINO_KOPECKS_STYLE);
  canvas.write(CASINO_KOPECKS_CAPTION,
               {right, top + CASINO_KOPECKS_CAPTION_TOP}, CASINO_CAPTION_STYLE);
  canvas.write(kopecksText(balances.chips), {right, top + CASINO_CHIPS_TOP},
               CASINO_CHIPS_STYLE);
  canvas.write(CASINO_CHIPS_CAPTION, {right, top + CASINO_CHIPS_CAPTION_TOP},
               CASINO_CAPTION_STYLE);
}

void paintButlerSpeech(IInterfaceCanvas& canvas, ButlerLine line) {
  const sf::IntRect area = CASINO_SPEECH_AREA;
  paintGoldPanel(canvas, area);
  paintNameTag(canvas,
               {area.position.x + CASINO_SPEECH_TAG_LEFT,
                area.position.y - (CASINO_TAG_HEIGHT / SIDES_PER_AXIS)},
               BUTLER_NAME);
  const sf::Vector2i first = area.position + CASINO_SPEECH_FIRST_LINE;
  canvas.write(line.first, first, CASINO_SPEECH_STYLE);
  canvas.write(line.second, {first.x, first.y + CASINO_SPEECH_LINE_GAP},
               CASINO_SPEECH_STYLE);
}

}  // namespace rtype::client
