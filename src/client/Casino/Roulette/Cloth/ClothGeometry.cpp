#include "ClothGeometry.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "ClothBand.hpp"
#include "ClothConstants.hpp"
#include "ClothPlace.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The size of a box of the cloth, @p inside between its lines, the
 * lines on both sides included. */
sf::Vector2i boxSizeAround(sf::Vector2i inside) {
  return inside + sf::Vector2i(1, 1);
}

/** @returns The column of the line before street @p street, the first line
 * of the numbers for street 0. */
int lineColumnBefore(int street) {
  return CLOTH_GRID_TOP_LEFT.x + (street * CLOTH_CELL_SIZE.x);
}

/** @returns The row of the line above row @p row of the numbers. */
int lineRowAbove(int row) {
  return CLOTH_GRID_TOP_LEFT.y + (row * CLOTH_CELL_SIZE.y);
}

/** @returns The column of the spots of @p across in the band of the
 * numbers. */
int spotColumnOf(int across) {
  if (across == ZERO_ACROSS) {
    return CLOTH_GRID_TOP_LEFT.x - (CLOTH_ZERO_WIDTH / SIDES_PER_AXIS);
  }
  if (across == NUMBERS_LAST_ACROSS) {
    return lineColumnBefore(CLOTH_STREETS) +
           (CLOTH_COLUMNS_WIDTH / SIDES_PER_AXIS);
  }
  if (across % 2 == 1) {
    return lineColumnBefore((across - 1) / 2) +
           (CLOTH_CELL_SIZE.x / SIDES_PER_AXIS);
  }
  return lineColumnBefore(across / 2);
}

/** @returns The row of the spots of @p down in the band of the numbers. */
int spotRowOf(int down) {
  if (down % ROW_STEP_DOWN == 0) {
    return lineRowAbove(down / ROW_STEP_DOWN) +
           (CLOTH_CELL_SIZE.y / SIDES_PER_AXIS);
  }
  return lineRowAbove((down + 1) / ROW_STEP_DOWN);
}

/** @returns The box of the outside bet of @p place, a dozen or an even
 * chance: as wide as its share of the twelve streets, under the numbers. */
sf::IntRect outsideBoxOf(ClothPlace place) {
  const bool dozen = place.band == ClothBand::Dozens;
  const int width =
      (CLOTH_STREETS * CLOTH_CELL_SIZE.x) /
      (dozen ? CLOTH_DOZENS : CLOTH_DOZENS * EVEN_CHANCES_PER_DOZEN);
  const int top =
      lineRowAbove(CLOTH_ROWS) + (dozen ? 0 : CLOTH_OUTSIDE_ROW_HEIGHT);
  return {{CLOTH_GRID_TOP_LEFT.x + (place.across * width), top},
          boxSizeAround({width, CLOTH_OUTSIDE_ROW_HEIGHT})};
}

/** @returns Whether @p place of the band of the numbers is a box rather than
 * a line or a corner. */
bool isBox(ClothPlace place) {
  return place.across == ZERO_ACROSS ||
         (place.down % ROW_STEP_DOWN == 0 && place.down != STREET_EDGE_DOWN &&
          (place.across % 2 == 1 || place.across == NUMBERS_LAST_ACROSS));
}

}  // namespace

sf::Vector2i spotOf(ClothPlace place) {
  if (place.band != ClothBand::Numbers) {
    const sf::IntRect box = outsideBoxOf(place);
    return box.position + (box.size / SIDES_PER_AXIS);
  }
  if (place.across == ZERO_ACROSS) {
    return {
        spotColumnOf(ZERO_ACROSS),
        lineRowAbove(0) + ((CLOTH_ROWS * CLOTH_CELL_SIZE.y) / SIDES_PER_AXIS)};
  }
  return {spotColumnOf(place.across), spotRowOf(place.down)};
}

sf::IntRect areaOf(ClothPlace place) {
  if (place.band != ClothBand::Numbers) {
    return outsideBoxOf(place);
  }
  if (place.across == ZERO_ACROSS) {
    return numberCellOf(0);
  }
  if (place.across == NUMBERS_LAST_ACROSS && isBox(place)) {
    return {{lineColumnBefore(CLOTH_STREETS),
             lineRowAbove(place.down / ROW_STEP_DOWN)},
            boxSizeAround({CLOTH_COLUMNS_WIDTH, CLOTH_CELL_SIZE.y})};
  }
  if (isBox(place)) {
    return {{lineColumnBefore((place.across - 1) / 2),
             lineRowAbove(place.down / ROW_STEP_DOWN)},
            boxSizeAround(CLOTH_CELL_SIZE)};
  }
  return {spotOf(place) - (CLOTH_LINE_SPOT_SIZE / SIDES_PER_AXIS),
          CLOTH_LINE_SPOT_SIZE};
}

sf::IntRect numberCellOf(std::uint8_t number) {
  if (number == 0) {
    return {{CLOTH_GRID_TOP_LEFT.x - CLOTH_ZERO_WIDTH, CLOTH_GRID_TOP_LEFT.y},
            boxSizeAround({CLOTH_ZERO_WIDTH, CLOTH_ROWS * CLOTH_CELL_SIZE.y})};
  }
  const int street = (number - 1) / CLOTH_ROWS;
  const int row = CLOTH_ROWS - 1 - ((number - 1) % CLOTH_ROWS);
  return {{lineColumnBefore(street), lineRowAbove(row)},
          boxSizeAround(CLOTH_CELL_SIZE)};
}

}  // namespace rtype::client
