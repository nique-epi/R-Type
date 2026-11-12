#include "ShopPainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <vector>
#include "ColoredPixel.hpp"
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "KopecksLabel.hpp"
#include "KopecksPainting.hpp"
#include "LookItemPurchase.hpp"
#include "Outline.hpp"
#include "PixelShapes.hpp"
#include "PriceTag.hpp"
#include "RenderingConstants.hpp"
#include "ScrollMark.hpp"
#include "ShipHulls.hpp"
#include "ShipLooks.hpp"
#include "ShipSheetFrame.hpp"
#include "ShopLayout.hpp"
#include "ShopLayoutConstants.hpp"
#include "ShopPaintingConstants.hpp"
#include "ShopShelf.hpp"
#include "ShopSpot.hpp"
#include "ShopSpriteConstants.hpp"
#include "ShopView.hpp"
#include "ShopWares.hpp"
#include "TextureArea.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Paints the red ribbon of a ware the player owns, across @p area
 * from its bottom-left corner upwards. */
void paintRibbon(IInterfaceCanvas& canvas, sf::IntRect area) {
  std::vector<ColoredPixel> ribbon;
  const int length = area.size.x - (SIDES_PER_AXIS * SHOP_RIBBON_INSET);
  ribbon.reserve(static_cast<std::size_t>(length) *
                 static_cast<std::size_t>(SHOP_RIBBON_THICKNESS));
  for (int step = 0; step < length; ++step) {
    const int row = area.position.y + area.size.y - SHOP_RIBBON_INSET -
                    SHOP_RIBBON_THICKNESS -
                    ((step * SHOP_RIBBON_RISE) / SHOP_RIBBON_RUN);
    for (int thick = 0; thick < SHOP_RIBBON_THICKNESS; ++thick) {
      ribbon.push_back({.position = {area.position.x + SHOP_RIBBON_INSET + step,
                                     row + thick},
                        .color = SHOP_RIBBON_COLOR});
    }
  }
  canvas.plot(ribbon);
}

/** @brief Paints the cyan frame of the chosen ware around @p area. */
void paintChosenFrame(IInterfaceCanvas& canvas, sf::IntRect area) {
  Outline(grownBy(area, SHOP_CHOSEN_FRAME_GAP), Outline::Line::Solid)
      .paint(canvas, HUD_CYAN);
  Outline(grownBy(area, SHOP_CHOSEN_FRAME_GAP - 1), Outline::Line::Solid)
      .paint(canvas, INK);
}

/** @brief Paints the look @p item in the cell @p cell: its icon, then its
 * tag, or its ribbon once owned. */
void paintLookCell(IInterfaceCanvas& canvas, sf::IntRect cell, std::size_t item,
                   bool owned) {
  paintLookIcon(
      canvas, item,
      {cell.position + SHOP_ICON_OFFSET, {SHOP_ICON_SIZE, SHOP_ICON_SIZE}});
  if (owned) {
    paintRibbon(canvas, cell);
    return;
  }
  paintPriceTag(canvas,
                {cell.position.x + cell.size.x - SHOP_CELL_TAG_INSET,
                 cell.position.y + SHOP_CELL_TAG_TOP},
                lookItemPrice(item));
}

/** @brief Paints the hull @p item in the stall @p stall: its ship in the
 * first pilot color, then its tag centred under it, or its ribbon once
 * owned. */
void paintHullStall(IInterfaceCanvas& canvas, sf::IntRect stall,
                    std::size_t item, bool owned) {
  const ShipSheetFrame ship = hullItemFrameOf(item);
  canvas.drawImage(
      TextureArea{.assetId = ship.sheet, .rectangle = rectangleOf(ship.frame)},
      ImagePlacement{.topLeft = {stall.position.x,
                                 stall.position.y + SHOP_STALL_SHIP_TOP}},
      INTERFACE_AREA);
  if (owned) {
    paintRibbon(canvas, stall);
    return;
  }
  const int width = priceTagWidth(lookItemPrice(item));
  paintPriceTag(canvas,
                {stall.position.x + ((stall.size.x + width) / HALF_DIVISOR),
                 stall.position.y + SHOP_STALL_TAG_TOP},
                lookItemPrice(item));
}

/** @brief Paints the looks of the rows the locker shows, and its scroll
 * mark. */
void paintLocker(IInterfaceCanvas& canvas, const ShopWares& wares,
                 const ShopView& view) {
  for (std::size_t slot = 0; slot < SHOP_LOCKER_SLOT_COUNT; ++slot) {
    const std::size_t look =
        (view.rows.first * SHOP_LOCKER_COLUMN_COUNT) + slot;
    if (look >= wares.looks.size()) {
      break;
    }
    const std::size_t item = wares.looks.at(look);
    paintLookCell(canvas, shopCellArea(slot), item,
                  view.owned.items.test(item));
    if (view.chosen == ShopSpot{.shelf = ShopShelf::Locker, .place = look}) {
      paintChosenFrame(canvas, shopCellArea(slot));
    }
  }
  paintScrollMark(canvas, SHOP_SCROLL_TRACK, view.rows);
}

/** @brief Paints the hulls of the glass case. */
void paintCase(IInterfaceCanvas& canvas, const ShopWares& wares,
               const ShopView& view) {
  for (std::size_t hull = 0; hull < wares.hulls.size(); ++hull) {
    const std::size_t item = wares.hulls.at(hull);
    paintHullStall(canvas, shopStallArea(hull), item,
                   view.owned.items.test(item));
    if (view.chosen == ShopSpot{.shelf = ShopShelf::HullCase, .place = hull}) {
      paintChosenFrame(canvas, shopStallArea(hull));
    }
  }
}

}  // namespace

void paintShop(IInterfaceCanvas& canvas, const ShopWares& wares,
               const ShopView& view) {
  canvas.drawImage(
      TextureArea{.assetId = SHOP_SCENE_ID, .rectangle = INTERFACE_AREA},
      ImagePlacement{.topLeft = INTERFACE_AREA.position}, INTERFACE_AREA);
  paintLocker(canvas, wares, view);
  paintCase(canvas, wares, view);
  paintKopecks(canvas, KopecksLabel{.topRight = SHOP_BALANCE_TOP_RIGHT,
                                    .amount = view.kopecks,
                                    .prefix = {},
                                    .outlined = true});
  canvas.write(view.status, SHOP_STATUS_ANCHOR, SHOP_STATUS_STYLE);
}

}  // namespace rtype::client
