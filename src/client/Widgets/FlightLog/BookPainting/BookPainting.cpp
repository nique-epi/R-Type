#include "BookPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <numbers>
#include "BookPose.hpp"
#include "Easing.hpp"
#include "EntitySprites.hpp"
#include "FlightLogColorConstants.hpp"
#include "FlightLogLayoutConstants.hpp"
#include "FlightLogSheetConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "SpriteFrame.hpp"
#include "TextureArea.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief One column of a turning half: the column of the sheet it shows,
 * where its top lands, and how far its ends spread. */
struct BookColumn {
  sf::IntRect source;
  sf::Vector2i topLeft;
  int rise{};
};

TextureArea sheetArea(sf::IntRect rectangle) {
  return TextureArea{.assetId = FLIGHT_LOG_SHEET_ID, .rectangle = rectangle};
}

void drawWhole(IInterfaceCanvas& canvas, game::SpriteFrame frame,
               sf::Vector2i topLeft) {
  canvas.drawImage(sheetArea(rectangleOf(frame)),
                   ImagePlacement{.topLeft = topLeft}, INTERFACE_AREA);
}

/** @brief Lays the dithered shadow over an area, tile by tile, the tiles cut
 * at its edges. */
void paintShadow(IInterfaceCanvas& canvas, sf::IntRect area) {
  const sf::IntRect tile = rectangleOf(SHADOW_TILE_FRAME);
  for (int row = 0; row < area.size.y; row += tile.size.y) {
    for (int column = 0; column < area.size.x; column += tile.size.x) {
      canvas.drawImage(
          sheetArea(tile),
          ImagePlacement{.topLeft = area.position + sf::Vector2i(column, row)},
          area);
    }
  }
}

/** @returns The color of the side of the closed book @p depth pixels out:
 * the back cover at the far end, then edges of pages and paper in turn. */
sf::Color sideColorAt(int depth) {
  if (depth == BOOK_SIDE_DEPTH) {
    return BOOK_EDGE_COLOR;
  }
  return depth % PAGE_STRIPE_PERIOD != 0 ? PAGE_EDGE_COLOR : PAGE_BLOCK_COLOR;
}

void paintClosed(IInterfaceCanvas& canvas, sf::Vector2i topLeft) {
  paintShadow(canvas,
              sf::IntRect(topLeft + BOOK_SHADOW_OFFSET,
                          BOOK_HALF_SIZE +
                              sf::Vector2i(BOOK_SIDE_DEPTH, BOOK_SIDE_DEPTH)));
  for (int depth = BOOK_SIDE_DEPTH; depth >= 1; --depth) {
    canvas.fill(
        sf::IntRect(topLeft + sf::Vector2i(depth, depth), BOOK_HALF_SIZE),
        sideColorAt(depth));
  }
  drawWhole(canvas, COVER_FRONT_FRAME, topLeft);
}

void paintOpen(IInterfaceCanvas& canvas, BookPose pose) {
  const sf::Vector2i left(pose.hinge - BOOK_HALF_SIZE.x, pose.top);
  paintShadow(canvas, sf::IntRect(left + BOOK_SHADOW_OFFSET,
                                  {SIDES_PER_AXIS * BOOK_HALF_SIZE.x,
                                   BOOK_HALF_SIZE.y}));
  drawWhole(canvas, COVER_INSIDE_FRAME, left);
  drawWhole(canvas, RIGHT_HALF_FRAME, {pose.hinge, pose.top});
  canvas.fill(
      sf::IntRect({pose.hinge - (SPINE_WIDTH / SIDES_PER_AXIS), pose.top},
                  {SPINE_WIDTH, BOOK_HALF_SIZE.y}),
      BOOK_EDGE_COLOR);
}

/** @brief Paints a column stretched by its rise at both ends: its upper half
 * moved up, its lower half moved down, the rows between them where they
 * are. */
void paintColumn(IInterfaceCanvas& canvas, const BookColumn& column) {
  const TextureArea area = sheetArea(column.source);
  if (column.rise == 0) {
    canvas.drawImage(area, ImagePlacement{.topLeft = column.topLeft},
                     INTERFACE_AREA);
    return;
  }
  const int left = column.topLeft.x;
  const int top = column.topLeft.y;
  const int height = column.source.size.y;
  const int half = height / SIDES_PER_AXIS;
  canvas.drawImage(area, ImagePlacement{.topLeft = {left, top - column.rise}},
                   sf::IntRect({left, top - column.rise}, {1, half}));
  canvas.drawImage(area, ImagePlacement{.topLeft = column.topLeft},
                   sf::IntRect({left, top + half - column.rise},
                               {1, SIDES_PER_AXIS * column.rise}));
  canvas.drawImage(
      area, ImagePlacement{.topLeft = {left, top + column.rise}},
      sf::IntRect({left, top + half + column.rise}, {1, height - half}));
}

void paintTurning(IInterfaceCanvas& canvas, BookPose pose) {
  const float angle = std::numbers::pi_v<float> * pose.open;
  const float cosine = std::cos(angle);
  const int width = pixelsOf(BOOK_HALF_SIZE.x, std::abs(cosine));
  const int lift = pixelsOf(COVER_LIFT, std::sin(angle));
  const bool front = cosine >= 0.0F;
  const sf::IntRect half =
      rectangleOf(front ? COVER_FRONT_FRAME : COVER_INSIDE_FRAME);
  paintShadow(canvas, sf::IntRect(sf::Vector2i(pose.hinge, pose.top) +
                                      BOOK_SHADOW_OFFSET,
                                  BOOK_HALF_SIZE));
  drawWhole(canvas, RIGHT_HALF_FRAME, {pose.hinge, pose.top});
  for (int column = 0; column < width; ++column) {
    const int fromHinge = (column * BOOK_HALF_SIZE.x) / width;
    const int source = front ? fromHinge : BOOK_HALF_SIZE.x - 1 - fromHinge;
    paintColumn(
        canvas,
        BookColumn{
            .source = sf::IntRect({half.position.x + source, half.position.y},
                                  {1, half.size.y}),
            .topLeft = {front ? pose.hinge + column : pose.hinge - 1 - column,
                        pose.top},
            .rise = (lift * (column + 1)) / width});
  }
}

}  // namespace

void paintBook(IInterfaceCanvas& canvas, BookPose pose) {
  if (pose.open <= 0.0F) {
    paintClosed(canvas, {pose.hinge, pose.top});
  } else if (pose.open >= 1.0F) {
    paintOpen(canvas, pose);
  } else {
    paintTurning(canvas, pose);
  }
}

}  // namespace rtype::client
