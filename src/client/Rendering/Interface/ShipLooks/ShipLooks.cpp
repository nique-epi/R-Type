#include "ShipLooks.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "RenderingConstants.hpp"
#include "ShipLook.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookNumbers.hpp"
#include "ShipLookSlot.hpp"
#include "ShipLookSpriteConstants.hpp"
#include "ShipPlacement.hpp"
#include "SpriteFrame.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

using identifiers::lookIndexOf;
using identifiers::lookNumberOf;
using identifiers::NO_SHIP_LOOK_ITEM;
using identifiers::SHIP_LOOK_ITEM_COUNT;
using identifiers::SHIP_LOOK_ITEM_SLOTS;
using identifiers::ShipLook;
using identifiers::ShipLookSlot;

namespace {

/** @returns The slot of @p look that @p slot names. */
std::uint8_t& slotOf(ShipLook& look, ShipLookSlot slot) {
  switch (slot) {
    case ShipLookSlot::Head:
      return look.head;
    case ShipLookSlot::Eyes:
      return look.eyes;
    case ShipLookSlot::Face:
      break;
  }
  return look.face;
}

}  // namespace

ShipLook wearing(ShipLook look, std::size_t item) {
  slotOf(look, SHIP_LOOK_ITEM_SLOTS.at(item)) = lookNumberOf(item);
  return look;
}

ShipLook without(ShipLook look, std::size_t item) {
  std::uint8_t& slot = slotOf(look, SHIP_LOOK_ITEM_SLOTS.at(item));
  if (slot == lookNumberOf(item)) {
    slot = NO_SHIP_LOOK_ITEM;
  }
  return look;
}

bool wears(ShipLook look, std::size_t item) {
  return slotOf(look, SHIP_LOOK_ITEM_SLOTS.at(item)) == lookNumberOf(item);
}

std::optional<std::size_t> itemInSlot(ShipLook look, ShipLookSlot slot) {
  const std::uint8_t number = slotOf(look, slot);
  if (number == NO_SHIP_LOOK_ITEM) {
    return std::nullopt;
  }
  return lookIndexOf(number);
}

std::vector<std::size_t> wornItemsOf(ShipLook look) {
  std::vector<std::size_t> worn;
  for (std::size_t item = 0; item < SHIP_LOOK_ITEM_COUNT; ++item) {
    if (wears(look, item)) {
      worn.push_back(item);
    }
  }
  return worn;
}

sf::IntRect lookRectangleOf(std::size_t item) {
  const game::SpriteFrame cell = SHIP_LOOK_CELLS.at(item);
  return {{cell.left, cell.top}, SHIP_LOOK_SPRITES.at(item).size};
}

void paintLookItem(IInterfaceCanvas& canvas, std::size_t item,
                   ShipPlacement ship) {
  canvas.drawImage(
      TextureArea{.assetId = SHIP_LOOK_SHEET_ID,
                  .rectangle = lookRectangleOf(item)},
      ImagePlacement{.topLeft = ship.topLeft +
                                (SHIP_LOOK_SPRITES.at(item).mount * ship.scale),
                     .scale = ship.scale},
      ship.visibleArea);
}

void paintShipLook(IInterfaceCanvas& canvas, ShipLook look,
                   ShipPlacement ship) {
  for (std::size_t item = 0; item < SHIP_LOOK_ITEM_COUNT; ++item) {
    if (wears(look, item)) {
      paintLookItem(canvas, item, ship);
    }
  }
}

void paintLookIcon(IInterfaceCanvas& canvas, std::size_t item,
                   sf::IntRect cell) {
  const sf::Vector2i size = SHIP_LOOK_SPRITES.at(item).size;
  const int scale =
      std::clamp(std::min(cell.size.x / size.x, cell.size.y / size.y),
                 NATIVE_SCALE, MAXIMUM_LOOK_ICON_SCALE);
  canvas.drawImage(
      TextureArea{.assetId = SHIP_LOOK_SHEET_ID,
                  .rectangle = lookRectangleOf(item)},
      ImagePlacement{.topLeft = cell.position +
                                ((cell.size - (size * scale)) / HALF_DIVISOR),
                     .scale = scale},
      cell);
}

}  // namespace rtype::client
