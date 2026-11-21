#include "HurtBeamPainter.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include "ActiveHurtsLaying.hpp"
#include "BeamLine.hpp"
#include "CanvasBox.hpp"
#include "CollisionConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "HurtBeamPainterConstants.hpp"
#include "HurtingEnemy.hpp"
#include "IPixelSurface.hpp"
#include "InterfacePixel.hpp"
#include "PaintedBeams.hpp"
#include "PixelGridConstants.hpp"
#include "Position.hpp"

namespace rtype::client {

namespace {

/** @returns The color of a pixel @p across logical units from the line of a
 * beam half @p halfThickness thick: the core, then outward to the rim. */
sf::Color bandColorOf(float across, float halfThickness) {
  const float share = across / halfThickness;
  if (share <= BEAM_CORE_SHARE) {
    return BEAM_WHITE;
  }
  if (share <= BEAM_INNER_SHARE) {
    return BEAM_PALE_PINK;
  }
  if (share <= BEAM_BODY_SHARE) {
    return BEAM_PINK;
  }
  return BEAM_DEEP_MAGENTA;
}

/** @returns The pixels of the canvas around both ends of @p line, half its
 * thickness wider on each side, kept inside the canvas. */
CanvasBox canvasBoxAround(const game::BeamLine& line) {
  const sf::Vector2f start(line.startX, line.startY);
  const sf::Vector2f end =
      start + (sf::Vector2f(line.heading.x, line.heading.y) * line.length);
  const sf::Vector2f margin(line.thickness * engine::HALF,
                            line.thickness * engine::HALF);
  const sf::Vector2i topLeft = interfacePixelAt(
      sf::Vector2f(std::min(start.x, end.x), std::min(start.y, end.y)) -
      margin);
  const sf::Vector2i bottomRight = interfacePixelAt(
      sf::Vector2f(std::max(start.x, end.x), std::max(start.y, end.y)) +
      margin);
  return CanvasBox{
      .left = std::max(0, topLeft.x),
      .top = std::max(0, topLeft.y),
      .right = std::min(static_cast<int>(BACKGROUND_WIDTH) - 1, bottomRight.x),
      .bottom =
          std::min(static_cast<int>(BACKGROUND_HEIGHT) - 1, bottomRight.y)};
}

/** @brief Plots the pixel @p pixel of the canvas when its center lies on
 * @p line, in the color of its distance to the line. */
void plotOnTheBand(IPixelSurface& surface, const game::BeamLine& line,
                   sf::Vector2i pixel) {
  const sf::Vector2f center =
      (sf::Vector2f(pixel) + PIXEL_CENTER) * BACKGROUND_PIXEL_SIZE;
  const float awayX = center.x - line.startX;
  const float awayY = center.y - line.startY;
  const float along = (awayX * line.heading.x) + (awayY * line.heading.y);
  const float across =
      std::abs((awayX * line.heading.y) - (awayY * line.heading.x));
  const float half = line.thickness * engine::HALF;
  if (along < 0.0F || along > line.length || across > half) {
    return;
  }
  surface.plot(pixel, bandColorOf(across, half));
}

/** @brief Paints @p line: each pixel of the canvas around it whose center
 * lies on its band. */
void paintBand(IPixelSurface& surface, const game::BeamLine& line) {
  const CanvasBox box = canvasBoxAround(line);
  for (int row = box.top; row <= box.bottom; ++row) {
    for (int column = box.left; column <= box.right; ++column) {
      plotOnTheBand(surface, line, sf::Vector2i(column, row));
    }
  }
}

}  // namespace

void HurtBeamPainter::paint(engine::ComponentRegistry& components,
                            IPixelSurface& surface) {
  components.forEach<PaintedBeams, game::Position>(
      [this, &surface](engine::Entity, const PaintedBeams& beams,
                       const game::Position& position) {
        game::layActiveHurts(game::HurtingEnemy{.zone = beams.zone,
                                                .position = position,
                                                .current = &beams.shown},
                             hurts_);
        for (const game::BeamLine& line : hurts_.beams) {
          paintBand(surface, line);
        }
      });
}

}  // namespace rtype::client
