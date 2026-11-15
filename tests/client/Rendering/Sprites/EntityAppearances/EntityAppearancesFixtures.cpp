#include "EntityAppearancesFixtures.hpp"
#include <SFML/Graphics/Image.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BackgroundConstants.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearances.hpp"
#include "EntityAppearancesTestConstants.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityTypesTestConstants.hpp"
#include "HurtBox.hpp"
#include "PixelMask.hpp"
#include "SheetPixels.hpp"
#include "SheetTextFixtures.hpp"
#include "ShippedEntitySheets.hpp"
#include "SpriteFrame.hpp"

using rtype::client::EntityAppearance;
using rtype::client::EntityAppearances;
using rtype::game::CLEAR_MASK_PIXEL;
using rtype::game::DRAWN_MASK_PIXEL;
using rtype::game::SpriteFrame;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::sheetText;
using rtype::game::testing::shippedSheetFolder;

namespace {

/** @returns Rows of @p width by @p height clear pixels. */
std::vector<std::string> clearRows(int width, int height) {
  std::vector<std::string> rows(
      static_cast<std::size_t>(height),
      std::string(static_cast<std::size_t>(width), CLEAR_MASK_PIXEL));
  return rows;
}

/** @returns The pixel of @p rows at @p column of @p line, which lies inside
 * them. */
char& pixelOf(std::vector<std::string>& rows, int column, int line) {
  return rows[static_cast<std::size_t>(line)][static_cast<std::size_t>(column)];
}

/** @brief Marks in @p rows each pixel a frame of @p frames of @p image
 * draws. */
void markDrawnPixels(const sf::Image& image,
                     std::span<const SpriteFrame> frames,
                     std::vector<std::string>& rows) {
  for (const SpriteFrame frame : frames) {
    for (int line = 0; line < frame.height; ++line) {
      for (int column = 0; column < frame.width; ++column) {
        if (isVisible(image, frame.left + column, frame.top + line)) {
          pixelOf(rows, column, line) = DRAWN_MASK_PIXEL;
        }
      }
    }
  }
}

/** @returns The pixels drawn in a frame of the idle or the enraged idle
 * animation of @p look, read in its PNG. */
std::vector<std::string> drawnRows(const EntityAppearance& look) {
  const sf::Image image = shippedSheet(look.sheet);
  const SpriteFrame first = look.idleFrames.front();
  std::vector<std::string> rows = clearRows(first.width, first.height);
  markDrawnPixels(image, look.idleFrames, rows);
  markDrawnPixels(image, look.enragedIdleFrames, rows);
  return rows;
}

/** @returns true when the pixel at @p column of @p line lies inside @p rows
 * and is drawn. */
bool isDrawnAt(const std::vector<std::string>& rows, int column, int line) {
  return line >= 0 && std::cmp_less(line, rows.size()) && column >= 0 &&
         std::cmp_less(column, rows.front().size()) &&
         rows[static_cast<std::size_t>(line)]
             [static_cast<std::size_t>(column)] == DRAWN_MASK_PIXEL;
}

/** @returns true when the four neighbours of the pixel at @p column of
 * @p line are drawn. */
bool hasEveryNeighbourDrawn(const std::vector<std::string>& rows, int column,
                            int line) {
  return isDrawnAt(rows, column - 1, line) &&
         isDrawnAt(rows, column + 1, line) &&
         isDrawnAt(rows, column, line - 1) && isDrawnAt(rows, column, line + 1);
}

/** @returns The column or row of a frame of @p side pixels where an edge
 * @p unitsFromCenter logical units from its center lies. */
float pixelEdgeOf(std::size_t side, float unitsFromCenter) {
  return (static_cast<float>(side) * rtype::engine::HALF) +
         (unitsFromCenter / rtype::client::BACKGROUND_PIXEL_SIZE);
}

/** @returns @p rows with every pixel outside @p box cleared, the box laid on
 * the rows as holdsTheHitbox() lays it. */
std::vector<std::string> cutToHitbox(std::vector<std::string> rows,
                                     const rtype::game::CollisionBox& box) {
  const float left = pixelEdgeOf(
      rows.front().size(), box.offsetX - (box.width * rtype::engine::HALF));
  const float right = left + (box.width / rtype::client::BACKGROUND_PIXEL_SIZE);
  const float top = pixelEdgeOf(
      rows.size(), box.offsetY - (box.height * rtype::engine::HALF));
  const float bottom =
      top + (box.height / rtype::client::BACKGROUND_PIXEL_SIZE);
  for (int line = 0; std::cmp_less(line, rows.size()); ++line) {
    for (int column = 0; std::cmp_less(column, rows.front().size()); ++column) {
      const auto across = static_cast<float>(column);
      const auto down = static_cast<float>(line);
      if (across < left || across >= right || down < top || down >= bottom) {
        pixelOf(rows, column, line) = CLEAR_MASK_PIXEL;
      }
    }
  }
  return rows;
}

}  // namespace

EntityAppearances appearancesReading(std::string_view text) {
  const std::array sheets{sheetText(READ_TYPE_NAME, text)};
  return EntityAppearances::fromTexts(sheets);
}

EntityAppearances threeAppearancesOnTwoSheets() {
  const std::array sheets{sheetText(READ_TYPE_NAME, READ_SHEET_TEXT),
                          sheetText(UNREAD_TYPE_NAME, OTHER_SHEET_TEXT),
                          sheetText(DRIFTER_SHEET, READ_SHEET_TEXT)};
  return EntityAppearances::fromTexts(sheets);
}

EntityAppearances shippedAppearances() {
  return EntityAppearances::fromFolder(shippedSheetFolder());
}

bool hurtsAllTheTime(const rtype::game::EntitySheet& sheet) {
  if (!sheet.hurtZone.has_value()) {
    return false;
  }
  return std::ranges::any_of(sheet.hurtZone->boxes,
                             [](const rtype::game::HurtBox& box) noexcept {
                               return !box.activeIn.has_value();
                             });
}

bool holds(rtype::game::SpriteFrame frame,
           const rtype::game::CollisionBox& box) {
  return box.width <= static_cast<float>(frame.width) *
                          rtype::client::BACKGROUND_PIXEL_SIZE &&
         box.height <= static_cast<float>(frame.height) *
                           rtype::client::BACKGROUND_PIXEL_SIZE;
}

bool hitsAnOffsetBox(const rtype::game::EntitySheet& sheet) {
  return sheet.hitbox.offsetX != 0.0F || sheet.hitbox.offsetY != 0.0F;
}

bool holdsTheHitbox(rtype::game::SpriteFrame frame,
                    const rtype::game::CollisionBox& box) {
  return holds(frame, rtype::game::HurtBox{.offsetX = box.offsetX,
                                           .offsetY = box.offsetY,
                                           .width = box.width,
                                           .height = box.height,
                                           .activeIn = std::nullopt});
}

bool holds(rtype::game::SpriteFrame frame, const rtype::game::HurtBox& box) {
  const float halfWidth = static_cast<float>(frame.width) *
                          rtype::client::BACKGROUND_PIXEL_SIZE *
                          rtype::engine::HALF;
  const float halfHeight = static_cast<float>(frame.height) *
                           rtype::client::BACKGROUND_PIXEL_SIZE *
                           rtype::engine::HALF;
  return std::abs(box.offsetX) + (box.width * rtype::engine::HALF) <=
             halfWidth &&
         std::abs(box.offsetY) + (box.height * rtype::engine::HALF) <=
             halfHeight;
}

std::vector<std::string> rowsOf(const rtype::engine::PixelMask& mask) {
  std::vector<std::string> rows = clearRows(mask.width(), mask.height());
  for (int line = 0; line < mask.height(); ++line) {
    for (int column = 0; column < mask.width(); ++column) {
      if (mask.isDrawn({.x = column, .y = line})) {
        pixelOf(rows, column, line) = DRAWN_MASK_PIXEL;
      }
    }
  }
  return rows;
}

std::vector<std::string> rowsFrom(std::span<const std::string_view> rows) {
  return {rows.begin(), rows.end()};
}

std::vector<std::string> peeled(std::vector<std::string> rows, int times) {
  for (int pass = 0; pass < times; ++pass) {
    std::vector<std::string> next = rows;
    for (int line = 0; std::cmp_less(line, rows.size()); ++line) {
      for (int column = 0; std::cmp_less(column, rows.front().size());
           ++column) {
        if (!hasEveryNeighbourDrawn(rows, column, line)) {
          pixelOf(next, column, line) = CLEAR_MASK_PIXEL;
        }
      }
    }
    rows = std::move(next);
  }
  return rows;
}

std::vector<std::string> expectedMaskRows(
    const EntityAppearance& look, int erosion,
    const rtype::game::CollisionBox& hitbox) {
  return cutToHitbox(peeled(drawnRows(look), erosion), hitbox);
}
