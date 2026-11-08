#include "LagometerPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include "BackgroundConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "Lagometer.hpp"
#include "LagometerConstants.hpp"
#include "LagometerLook.hpp"
#include "LagometerPoint.hpp"
#include "LagometerText.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

namespace {

/** @brief The rectangle the curve covers, in coordinates of the playfield. */
struct CurveArea {
  float left{};
  float right{};
  float top{};
  float bottom{};
};

/** @returns @p pixels of the interface picture, at @p scale, in coordinates
 * of the playfield. */
float playfieldLength(int pixels, int scale) {
  return static_cast<float>(pixels * scale) * BACKGROUND_PIXEL_SIZE;
}

/** @returns The rectangle the curve covers at @p scale. */
CurveArea curveAreaAt(int scale) {
  const float left = playfieldLength(LAGOMETER_MARGIN, scale) +
                     playfieldLength(LAGOMETER_TOP_LEFT.x, 1);
  const float top = playfieldLength(LAGOMETER_CURVE_TOP, scale) +
                    playfieldLength(LAGOMETER_TOP_LEFT.y, 1);
  return CurveArea{
      .left = left,
      .right =
          left + playfieldLength(LAGOMETER_SIZE.x - (2 * LAGOMETER_MARGIN) -
                                     LAGOMETER_SCALE_LABEL_WIDTH,
                                 scale),
      .top = top,
      .bottom = top + playfieldLength(LAGOMETER_CURVE_HEIGHT, scale)};
}

/** @brief Adds a line from @p from to @p to, in @p color. */
void addLine(sf::VertexArray& lines, sf::Vector2f from, sf::Vector2f to,
             sf::Color color) {
  lines.append(sf::Vertex{.position = from, .color = color});
  lines.append(sf::Vertex{.position = to, .color = color});
}

/** @brief Adds the lines of the scale: at 0, half the top, and the top. */
void addScaleLines(sf::VertexArray& lines, const CurveArea& area) {
  const float middle = (area.top + area.bottom) / 2.0F;
  for (const float row : {area.bottom, middle, area.top}) {
    addLine(lines, {area.left, row}, {area.right, row}, LAGOMETER_SCALE_LINE);
  }
}

/** @returns The x of the point at @p index of @p count, the newest on the
 * right edge of @p area, one step of the full curve apart. */
float pointX(const CurveArea& area, std::size_t index, std::size_t count) {
  const float step =
      (area.right - area.left) / static_cast<float>(LAGOMETER_POINT_COUNT - 1);
  return area.right - (static_cast<float>(count - 1 - index) * step);
}

/** @returns The y of @p roundTrip on the scale up to @p top, held at the top
 * above it. */
float pointY(const CurveArea& area, std::chrono::microseconds roundTrip,
             std::chrono::milliseconds top) {
  const double share = std::min(1.0, std::chrono::duration<double>(roundTrip) /
                                         std::chrono::duration<double>(top));
  return area.bottom - (static_cast<float>(share) * (area.bottom - area.top));
}

/** @brief Adds a red line across the curve at each point that missed a
 * datagram, then the line from each point to the next. */
void addPoints(sf::VertexArray& lines, const CurveArea& area,
               const Lagometer& lagometer) {
  const std::deque<LagometerPoint>& points = lagometer.points();
  const std::chrono::milliseconds top = lagometer.scaleTop();
  for (std::size_t index = 0; index < points.size(); ++index) {
    if (points[index].missed > 0) {
      const float x = pointX(area, index, points.size());
      addLine(lines, {x, area.top}, {x, area.bottom}, LOSS_RED);
    }
  }
  for (std::size_t index = 1; index < points.size(); ++index) {
    addLine(lines,
            {pointX(area, index - 1, points.size()),
             pointY(area, points[index - 1].roundTrip, top)},
            {pointX(area, index, points.size()),
             pointY(area, points[index].roundTrip, top)},
            LAGOMETER_CURVE);
  }
}

/** @returns The style of the text of the panel at @p scale. */
TextStyle panelTextStyle(int scale) {
  return TextStyle{.font = TextFont::Small,
                   .color = LAGOMETER_TEXT,
                   .outlined = false,
                   .alignment = TextAlignment::Left,
                   .scale = scale,
                   .clip = std::nullopt};
}

/** @brief Paints the fill and the border of the panel. */
void paintFrame(IInterfaceCanvas& canvas, LagometerLook look) {
  const sf::Vector2i size = LAGOMETER_SIZE * look.scale;
  const int border = LAGOMETER_BORDER_WIDTH * look.scale;
  sf::Color fill = LAGOMETER_FILL;
  fill.a = static_cast<std::uint8_t>(LAGOMETER_OPAQUE_ALPHA * look.fillPercent /
                                     static_cast<int>(PERCENT));
  canvas.fill(sf::IntRect(LAGOMETER_TOP_LEFT, size), fill);
  canvas.fill(sf::IntRect(LAGOMETER_TOP_LEFT, {size.x, border}),
              LAGOMETER_BORDER);
  canvas.fill(sf::IntRect(LAGOMETER_TOP_LEFT + sf::Vector2i{0, size.y - border},
                          {size.x, border}),
              LAGOMETER_BORDER);
  canvas.fill(sf::IntRect(LAGOMETER_TOP_LEFT, {border, size.y}),
              LAGOMETER_BORDER);
  canvas.fill(sf::IntRect(LAGOMETER_TOP_LEFT + sf::Vector2i{size.x - border, 0},
                          {border, size.y}),
              LAGOMETER_BORDER);
}

/** @brief Writes the top of the scale and its half on the right of the
 * curve. */
void paintScaleLabels(IInterfaceCanvas& canvas, const Lagometer& lagometer,
                      int scale) {
  const sf::Vector2i labelLeft =
      LAGOMETER_TOP_LEFT +
      (sf::Vector2i{LAGOMETER_SIZE.x - LAGOMETER_MARGIN -
                        LAGOMETER_SCALE_LABEL_WIDTH + LAGOMETER_MARGIN,
                    LAGOMETER_CURVE_TOP} *
       scale);
  const std::chrono::milliseconds top = lagometer.scaleTop();
  canvas.write(std::to_string(top.count()), labelLeft, panelTextStyle(scale));
  canvas.write(std::to_string((top / 2).count()),
               labelLeft + sf::Vector2i{0, LAGOMETER_CURVE_HEIGHT / 2 * scale},
               panelTextStyle(scale));
}

}  // namespace

void paintLagometerPanel(IInterfaceCanvas& canvas, const Lagometer& lagometer,
                         LagometerLook look) {
  paintFrame(canvas, look);
  const auto lines = lagometerTextLines(lagometer.figures());
  for (std::size_t line = 0; line < lines.size(); ++line) {
    const sf::Vector2i anchor =
        LAGOMETER_TOP_LEFT +
        (sf::Vector2i{LAGOMETER_MARGIN,
                      LAGOMETER_MARGIN +
                          (static_cast<int>(line) * LAGOMETER_LINE_STEP)} *
         look.scale);
    canvas.write(lines.at(line), anchor, panelTextStyle(look.scale));
  }
  paintScaleLabels(canvas, lagometer, look.scale);
}

sf::VertexArray lagometerCurve(const Lagometer& lagometer, int scale) {
  sf::VertexArray lines(sf::PrimitiveType::Lines);
  const CurveArea area = curveAreaAt(scale);
  addScaleLines(lines, area);
  addPoints(lines, area, lagometer);
  return lines;
}

}  // namespace rtype::client
