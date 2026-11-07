#include "LockerPainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "ColoredPixel.hpp"
#include "DitherPatch.hpp"
#include "GlassStreak.hpp"
#include "HangarColorConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "LockerChoice.hpp"
#include "LockerConstants.hpp"
#include "LockerView.hpp"
#include "MetalPatch.hpp"
#include "MetalShades.hpp"
#include "MetalTexture.hpp"
#include "MetalTextureConstants.hpp"
#include "Outline.hpp"
#include "PixelShapes.hpp"
#include "RenderingConstants.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLooks.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

using identifiers::SHIP_LOOK_ITEM_COUNT;

namespace {

/** @brief Paints the four sides of @p area: @p shades.light on the top and
 * left, @p shades.dark on the bottom and right. */
void paintBevel(IInterfaceCanvas& canvas, sf::IntRect area,
                MetalShades shades) {
  const sf::Vector2i end = area.position + area.size - sf::Vector2i(1, 1);
  canvas.fill({area.position, {area.size.x, 1}}, shades.light);
  canvas.fill({area.position, {1, area.size.y}}, shades.light);
  canvas.fill({{area.position.x, end.y}, {area.size.x, 1}}, shades.dark);
  canvas.fill({{end.x, area.position.y}, {1, area.size.y}}, shades.dark);
}

/** @brief Adds a rivet near each corner of @p area. */
void addCornerRivets(std::vector<ColoredPixel>& pixels, sf::IntRect area) {
  const sf::Vector2i first = area.position + RIVET_OFFSET;
  const sf::Vector2i last = area.position + area.size - RIVET_INSET;
  addRivet(pixels, first);
  addRivet(pixels, {last.x, first.y});
  addRivet(pixels, {first.x, last.y});
  addRivet(pixels, last);
}

void paintBody(IInterfaceCanvas& canvas, sf::IntRect body) {
  canvas.fill(grownBy(body, 1), INK);
  canvas.fill(body, DEEP_BLUE);
  std::vector<ColoredPixel> pixels;
  addBrushStrokes(pixels,
                  MetalPatch{.area = body,
                             .shades = {.light = BLUE, .dark = STEEL_DARK},
                             .seed = LOCKER_BODY_SEED});
  addWear(pixels, MetalPatch{.area = body,
                             .shades = {.light = PALE_BLUE, .dark = STEEL_DARK},
                             .seed = LOCKER_WEAR_SEED});
  addCornerRivets(pixels, body);
  canvas.plot(pixels);
  paintBevel(canvas, body, {.light = PALE_BLUE, .dark = STEEL_DARK});
}

/** @returns Where the name plate lies: centred on the top of @p body, as wide
 * as its name needs and NAMEPLATE_MINIMUM_WIDTH at least. */
sf::IntRect nameplateArea(const IInterfaceCanvas& canvas, sf::IntRect body) {
  const int width = std::max(
      NAMEPLATE_MINIMUM_WIDTH,
      canvas.textWidth(LOCKER_TITLE, TextFont::Small) + NAMEPLATE_MARGIN);
  return {{body.position.x + ((body.size.x - width) / HALF_DIVISOR),
           body.position.y + NAMEPLATE_TOP},
          {width, NAMEPLATE_HEIGHT}};
}

/** @brief Paints the plate itself: two dark brackets holding it, brushed and
 * worn steel shaded towards its bottom, four rivets, a raised edge. */
void paintNameplateMetal(IInterfaceCanvas& canvas, sf::IntRect plate) {
  const int bracketTop = plate.position.y + NAMEPLATE_BRACKET_TOP;
  canvas.fill({{plate.position.x - NAMEPLATE_BRACKET_SIZE.x, bracketTop},
               NAMEPLATE_BRACKET_SIZE},
              INK);
  canvas.fill(
      {{plate.position.x + plate.size.x, bracketTop}, NAMEPLATE_BRACKET_SIZE},
      INK);
  canvas.fill(grownBy(plate, 1), INK);
  canvas.fill(plate, PALE_BLUE);
  std::vector<ColoredPixel> pixels;
  addBrushStrokes(pixels,
                  MetalPatch{.area = plate,
                             .shades = {.light = STAR_WHITE, .dark = BLUE},
                             .seed = NAMEPLATE_SEED});
  addDither(pixels, DitherPatch{.area = plate,
                                .color = BLUE,
                                .topShare = 0.0F,
                                .bottomShare = NAMEPLATE_SHADE_SHARE});
  addWear(pixels, MetalPatch{.area = plate,
                             .shades = {.light = BLUE, .dark = DEEP_BLUE},
                             .seed = NAMEPLATE_SEED});
  addCornerRivets(pixels, plate);
  canvas.plot(pixels);
  paintBevel(canvas, plate, {.light = STAR_WHITE, .dark = DEEP_BLUE});
}

/** @brief Writes the name engraved in the plate: dark letters over a lit
 * copy one pixel lower and to the right, as a cut in metal catches the light.
 */
void paintNameplateName(IInterfaceCanvas& canvas, sf::IntRect plate) {
  const sf::Vector2i anchor(plate.position.x + (plate.size.x / HALF_DIVISOR),
                            plate.position.y + NAMEPLATE_TEXT_TOP);
  canvas.write(LOCKER_TITLE, anchor + sf::Vector2i(1, 1),
               {.font = TextFont::Small,
                .color = STAR_WHITE,
                .alignment = TextAlignment::Center});
  canvas.write(LOCKER_TITLE, anchor,
               {.font = TextFont::Small,
                .color = INK,
                .alignment = TextAlignment::Center});
}

/** @brief Paints a cell without its item: dark steel shaded under its top, a
 * sunk edge, a shelf. */
void paintCellBack(IInterfaceCanvas& canvas, sf::IntRect cell) {
  canvas.fill(cell, STEEL_DARK);
  std::vector<ColoredPixel> shadow;
  addDither(shadow, DitherPatch{.area = {cell.position,
                                         {cell.size.x, CELL_SHADOW_ROWS}},
                                .color = DEEP_BLUE,
                                .topShare = CELL_SHADOW_SHARE,
                                .bottomShare = 0.0F});
  canvas.plot(shadow);
  paintBevel(canvas, cell, {.light = HANGAR_SHADOW, .dark = BLUE});
  canvas.fill(
      {cell.position +
           sf::Vector2i(CELL_SHELF_INSET, cell.size.y - CELL_SHELF_INSET),
       {cell.size.x - CELL_SHELF_INSET - CELL_SHELF_INSET, CELL_SHELF_HEIGHT}},
      DEEP_BLUE);
}

/** @brief Paints the item of @p slot when the locker holds it, its gold tag
 * when the ship wears it, and the cyan frame when it is the chosen one, held
 * or not. */
void paintCellItem(IInterfaceCanvas& canvas, std::size_t slot,
                   LockerView view) {
  const sf::IntRect cell = lockerCellArea(slot);
  const bool held = view.locker.items.test(slot);
  if (held) {
    paintLookIcon(canvas, slot, grownBy(cell, -CELL_ICON_INSET));
  }
  if (held && wears(view.worn, slot)) {
    const sf::Vector2i tag(
        cell.position.x + cell.size.x - WORN_TAG_INSET - WORN_TAG_SIZE.x,
        cell.position.y + WORN_TAG_INSET);
    canvas.fill(grownBy({tag, WORN_TAG_SIZE}, 1), INK);
    canvas.fill({tag, WORN_TAG_SIZE}, STAR_GOLD);
  }
  if (slot == view.chosen) {
    Outline(grownBy(cell, CHOSEN_FRAME_GAP), Outline::Line::Solid)
        .paint(canvas, HUD_CYAN);
    Outline(grownBy(cell, CHOSEN_FRAME_GAP - 1), Outline::Line::Solid)
        .paint(canvas, INK);
  }
}

/** @brief Adds the pixels of @p streak across @p inside: a diagonal leaning
 * GLASS_STREAK_SLOPE per row, one pixel in two lit by the dither. */
void addStreak(std::vector<ColoredPixel>& pixels, sf::IntRect inside,
               GlassStreak streak) {
  const auto height = static_cast<float>(inside.size.y);
  const int start =
      inside.position.x +
      static_cast<int>(streak.start * static_cast<float>(inside.size.x)) -
      static_cast<int>(GLASS_STREAK_SLOPE * height /
                       static_cast<float>(HALF_DIVISOR));
  for (int row = 0; row < inside.size.y; ++row) {
    for (int column = 0; column < streak.width; ++column) {
      const sf::Vector2i pixel(
          start +
              static_cast<int>(GLASS_STREAK_SLOPE * static_cast<float>(row)) +
              column,
          inside.position.y + row);
      if (inside.contains(pixel) &&
          bayerThresholdAt(pixel) < GLASS_STREAK_DENSITY) {
        pixels.push_back({.position = pixel, .color = PALE_BLUE});
      }
    }
  }
}

void paintGlass(IInterfaceCanvas& canvas, sf::IntRect body) {
  const sf::IntRect door(
      body.position + sf::Vector2i(GLASS_DOOR_INSET, LOCKER_PLATE_HEIGHT -
                                                         GLASS_DOOR_INSET +
                                                         GLASS_DOOR_RISE),
      body.size - sf::Vector2i(GLASS_DOOR_INSET + GLASS_DOOR_INSET,
                               LOCKER_PLATE_HEIGHT));
  Outline(door, Outline::Line::Solid).paint(canvas, PALE_BLUE);
  Outline(grownBy(door, -1), Outline::Line::Solid).paint(canvas, STEEL_DARK);
  std::vector<ColoredPixel> pixels;
  for (const GlassStreak& streak : GLASS_STREAKS) {
    addStreak(pixels, grownBy(door, -2), streak);
  }
  canvas.plot(pixels);
  const sf::IntRect handle(
      {door.position.x + door.size.x - GLASS_HANDLE_INSET,
       door.position.y + ((door.size.y - GLASS_HANDLE_SIZE.y) / HALF_DIVISOR)},
      GLASS_HANDLE_SIZE);
  canvas.fill(handle, INK);
  canvas.fill(
      {handle.position + sf::Vector2i(1, 1), {1, GLASS_HANDLE_SIZE.y - 2}},
      PALE_BLUE);
}

}  // namespace

void paintLocker(IInterfaceCanvas& canvas, LockerView view) {
  const sf::IntRect body(LOCKER_TOP_LEFT, LOCKER_SIZE);
  paintBody(canvas, body);
  const sf::IntRect plate = nameplateArea(canvas, body);
  paintNameplateMetal(canvas, plate);
  paintNameplateName(canvas, plate);
  for (std::size_t slot = 0; slot < LOCKER_SLOT_COUNT; ++slot) {
    paintCellBack(canvas, lockerCellArea(slot));
    if (slot < SHIP_LOOK_ITEM_COUNT) {
      paintCellItem(canvas, slot, view);
    }
  }
  paintGlass(canvas, body);
}

}  // namespace rtype::client
