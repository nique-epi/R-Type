#include "RowScrolling.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "BackgroundConstants.hpp"
#include "ColoredPixel.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfacePixel.hpp"
#include "PlayfieldMapping.hpp"
#include "RowScroll.hpp"
#include "RowWindow.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The row of @p scroll that @p button is, or nothing when it is not
 * one of its rows. */
std::optional<std::size_t> rowOf(const RowScroll& scroll, std::size_t button) {
  if (button < scroll.firstButton ||
      button >= scroll.firstButton + scroll.rows.count) {
    return std::nullopt;
  }
  return button - scroll.firstButton;
}

}  // namespace

void paintScrollArrow(IInterfaceCanvas& canvas, sf::Vector2i tip, int towards) {
  std::vector<ColoredPixel> pixels;
  for (int row = 0; row < SCROLL_MARK_ROWS; ++row) {
    for (int column = -row; column <= row; ++column) {
      pixels.push_back(
          ColoredPixel{.position = tip + sf::Vector2i(column, row * towards),
                       .color = SCROLL_MARK_COLOR});
    }
  }
  canvas.plot(pixels);
}

sf::Vector2i rowOffsetOf(const RowScroll& scroll) {
  return {0, -static_cast<int>(scroll.rows.first) * scroll.pitch};
}

bool isHiddenRow(const RowScroll& scroll, std::size_t button) {
  const std::optional<std::size_t> row = rowOf(scroll, button);
  return row.has_value() && !showsRow(scroll.rows, *row);
}

RowScroll scrollShowing(RowScroll scroll, std::size_t button) {
  const std::optional<std::size_t> row = rowOf(scroll, button);
  if (row.has_value()) {
    scroll.rows = rowsShowing(scroll.rows, *row);
  }
  return scroll;
}

RowScroll scrollMovedBy(RowScroll scroll, int step) {
  scroll.rows = rowsScrolledBy(scroll.rows, step);
  return scroll;
}

PlayfieldMapping shiftedMappingOf(sf::Vector2i offset,
                                  const PlayfieldMapping& mapping) {
  return [offset, mapping](sf::Vector2i windowPixel) {
    return mapping(windowPixel) +
           (sf::Vector2f(offset) * BACKGROUND_PIXEL_SIZE);
  };
}

PlayfieldMapping scrolledMappingOf(const std::optional<RowScroll>& scroll,
                                   const PlayfieldMapping& mapping) {
  if (!scroll.has_value()) {
    return mapping;
  }
  return [viewport = scroll->viewport, offset = rowOffsetOf(*scroll),
          mapping](sf::Vector2i windowPixel) {
    const sf::Vector2f point = mapping(windowPixel);
    if (!viewport.contains(interfacePixelAt(point))) {
      return point;
    }
    return point - (sf::Vector2f(offset) * BACKGROUND_PIXEL_SIZE);
  };
}

}  // namespace rtype::client
