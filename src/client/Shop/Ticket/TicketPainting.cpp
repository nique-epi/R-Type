#include "TicketPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "ColoredPixel.hpp"
#include "EntitySprites.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "LookItemDescriptionConstants.hpp"
#include "LookItemPurchase.hpp"
#include "PurchaseTicketConstants.hpp"
#include "RenderingConstants.hpp"
#include "ShipHulls.hpp"
#include "ShipLookNumbers.hpp"
#include "ShipLooks.hpp"
#include "ShipSheetFrame.hpp"
#include "ShopSpriteConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TicketButton.hpp"
#include "TicketView.hpp"

namespace rtype::client {

namespace {

/** @returns @p point of the ticket, counted from its top-left corner, as a
 * pixel of the interface picture. */
sf::Vector2i onTicket(sf::Vector2i point) {
  return TICKET_AREA.position + point;
}

/** @returns @p area of the ticket as an area of the interface picture. */
sf::IntRect onTicket(sf::IntRect area) {
  return {onTicket(area.position), area.size};
}

/** @brief Writes @p text at @p anchor of the ticket. */
void writeOnTicket(IInterfaceCanvas& canvas, std::string_view text,
                   sf::Vector2i anchor, TextStyle style) {
  canvas.write(text, onTicket(anchor), style);
}

/** @brief Lays @p rectangle of the ticket sheet at @p topLeft of the
 * ticket. */
void layPicture(IInterfaceCanvas& canvas, sf::IntRect rectangle,
                sf::Vector2i topLeft) {
  canvas.drawImage(
      TextureArea{.assetId = SHOP_TICKET_SHEET_ID, .rectangle = rectangle},
      ImagePlacement{.topLeft = onTicket(topLeft)}, INTERFACE_AREA);
}

/** @brief Lays the paper, whole, or cut at its torn edge once paid. */
void paintPaper(IInterfaceCanvas& canvas, bool paid) {
  if (!paid) {
    layPicture(canvas, TICKET_PAPER_RECTANGLE, {0, 0});
    return;
  }
  layPicture(canvas,
             {TICKET_PAPER_RECTANGLE.position,
              {TICKET_PAPER_RECTANGLE.size.x, TICKET_TORN_EDGE_TOP}},
             {0, 0});
  layPicture(canvas, TICKET_TORN_EDGE_RECTANGLE, {0, TICKET_TORN_EDGE_TOP});
}

/** @brief Prints a red rule across the form at @p row, every
 * TICKET_DOT_PITCH pixel only when @p dotted. */
void paintRule(IInterfaceCanvas& canvas, int row, bool dotted) {
  if (!dotted) {
    canvas.fill(
        onTicket(sf::IntRect{{TICKET_RULE_LEFT, row}, {TICKET_RULE_WIDTH, 1}}),
        TICKET_RED);
    return;
  }
  std::vector<ColoredPixel> dots;
  dots.reserve(static_cast<std::size_t>(TICKET_RULE_WIDTH / TICKET_DOT_PITCH));
  for (int column = 0; column < TICKET_RULE_WIDTH; column += TICKET_DOT_PITCH) {
    dots.push_back({.position = onTicket({TICKET_RULE_LEFT + column, row}),
                    .color = TICKET_RED});
  }
  canvas.plot(dots);
}

/** @brief Prints the form: its rules, the store, the series and the
 * heading. */
void paintForm(IInterfaceCanvas& canvas) {
  for (const int row : {TICKET_TOP_RULE_ROW, TICKET_SECOND_TOP_RULE_ROW,
                        TICKET_HEADING_RULE_ROW, TICKET_PRICE_RULE_ROW}) {
    paintRule(canvas, row, false);
  }
  paintRule(canvas, TICKET_DOTTED_RULE_ROW, true);
  writeOnTicket(canvas, TICKET_STORE_LABEL, {TICKET_MARGIN, TICKET_FORM_ROW},
                {.font = TextFont::Small, .color = TICKET_RED});
  writeOnTicket(canvas, TICKET_SERIES_LABEL,
                {TICKET_SIZE.x - TICKET_MARGIN, TICKET_FORM_ROW},
                {.font = TextFont::Regular,
                 .color = TICKET_RED,
                 .alignment = TextAlignment::Right});
  writeOnTicket(canvas, TICKET_HEADING, {TICKET_MIDDLE, TICKET_HEADING_ROW},
                {.font = TextFont::Regular,
                 .color = TICKET_DARK_RED,
                 .alignment = TextAlignment::Center});
}

/** @brief Draws the ware @p item centred in TICKET_WARE_AREA: the icon of an
 * accessory, the ship of a hull, as large as each fits. */
void paintWare(IInterfaceCanvas& canvas, std::size_t item) {
  const sf::IntRect area = onTicket(TICKET_WARE_AREA);
  if (!identifiers::isHullItem(item)) {
    paintLookIcon(canvas, item, area);
    return;
  }
  const ShipSheetFrame ship = hullItemFrameOf(item);
  const sf::IntRect frame = rectangleOf(ship.frame);
  const int scale = std::max(
      NATIVE_SCALE,
      std::min(area.size.x / frame.size.x, area.size.y / frame.size.y));
  canvas.drawImage(
      TextureArea{.assetId = ship.sheet, .rectangle = frame},
      ImagePlacement{
          .topLeft = area.position +
                     ((area.size - (frame.size * scale)) / HALF_DIVISOR),
          .scale = scale},
      area);
}

/** @brief Types the name and the description of @p item under it. */
void paintWords(IInterfaceCanvas& canvas, std::size_t item) {
  const TextStyle typed{.font = TextFont::Regular,
                        .color = TICKET_INK,
                        .alignment = TextAlignment::Center};
  writeOnTicket(canvas, lookItemLabelOf(item), {TICKET_MIDDLE, TICKET_NAME_ROW},
                typed);
  int row = TICKET_DESCRIPTION_ROW;
  for (const std::string_view line : LOOK_ITEM_DESCRIPTIONS.at(item)) {
    writeOnTicket(canvas, line, {TICKET_MIDDLE, row}, typed);
    row += TICKET_DESCRIPTION_LINE_HEIGHT;
  }
}

/** @returns The price of @p item as the ticket types it. */
std::string priceTextOf(std::size_t item) {
  return std::to_string(lookItemPrice(item)) + std::string(TICKET_PRICE_SUFFIX);
}

/** @brief Types the quantity and the price of @p item. */
void paintPrice(IInterfaceCanvas& canvas, std::size_t item) {
  const TextStyle label{.font = TextFont::Small, .color = TICKET_RED};
  writeOnTicket(canvas, TICKET_QUANTITY_LABEL,
                {TICKET_MARGIN, TICKET_LABEL_ROW}, label);
  writeOnTicket(canvas, TICKET_PRICE_LABEL,
                {TICKET_PRICE_LABEL_LEFT, TICKET_LABEL_ROW}, label);
  writeOnTicket(canvas, priceTextOf(item),
                {TICKET_SIZE.x - TICKET_MARGIN, TICKET_PRICE_ROW},
                {.font = TextFont::Regular,
                 .color = TICKET_INK,
                 .alignment = TextAlignment::Right});
}

/** @brief Paints the cyan corners of the choice around @p face. */
void paintChoiceCorners(IInterfaceCanvas& canvas, sf::IntRect face) {
  const int left = face.position.x - TICKET_CORNER_GAP;
  const int top = face.position.y - TICKET_CORNER_GAP;
  const int right = face.position.x + face.size.x + TICKET_CORNER_GAP - 1;
  const int bottom = face.position.y + face.size.y + TICKET_CORNER_GAP - 1;
  for (const int column : {left, right - TICKET_CORNER_LENGTH + 1}) {
    for (const int row : {top, bottom}) {
      canvas.fill({{column, row}, {TICKET_CORNER_LENGTH, 1}}, HUD_CYAN);
    }
  }
  for (const int column : {left, right}) {
    for (const int row : {top, bottom - TICKET_CORNER_LENGTH + 1}) {
      canvas.fill({{column, row}, {1, TICKET_CORNER_LENGTH}}, HUD_CYAN);
    }
  }
}

/** @brief How a button of the ticket looks: its area, its label and key,
 * whether it is the full red one, and whether it is chosen. */
struct TicketButtonLook {
  sf::IntRect area;
  std::string_view label;
  std::string_view key;
  bool full{false};
  bool chosen{false};
};

/** @brief Paints a button: its shadow, its face, sunk once chosen, its
 * border, its label and its key, then the corners of the choice. */
void paintButton(IInterfaceCanvas& canvas, const TicketButtonLook& look) {
  const sf::IntRect area = onTicket(look.area);
  canvas.fill({area.position + sf::Vector2i{1, area.size.y},
               {area.size.x, TICKET_BUTTON_SHADOW}},
              TICKET_DARK_RED);
  canvas.fill({area.position + sf::Vector2i{area.size.x, 1}, {1, area.size.y}},
              TICKET_DARK_RED);
  const sf::IntRect face{
      area.position + sf::Vector2i{0, look.chosen ? TICKET_BUTTON_SINK : 0},
      area.size};
  canvas.fill(face, look.full ? TICKET_DARK_RED : TICKET_RED);
  sf::Color inside =
      look.chosen ? TICKET_CHOSEN_CANCEL_FACE : TICKET_CANCEL_FACE;
  if (look.full) {
    inside = look.chosen ? TICKET_CHOSEN_BUY_FACE : TICKET_BUY_FACE;
  }
  canvas.fill(
      {face.position + sf::Vector2i{1, 1}, face.size - sf::Vector2i{2, 2}},
      inside);
  canvas.fill({face.position + sf::Vector2i{1, 1}, {face.size.x - 2, 1}},
              TICKET_BUTTON_LIGHT);
  const int middle = face.position.x + (face.size.x / HALF_DIVISOR);
  canvas.write(
      look.label, {middle, face.position.y + TICKET_BUTTON_LABEL_ROW},
      {.font = TextFont::Regular,
       .color = look.full ? TICKET_CHOSEN_CANCEL_FACE : TICKET_DARK_RED,
       .alignment = TextAlignment::Center});
  canvas.write(look.key, {middle, face.position.y + TICKET_BUTTON_KEY_ROW},
               {.font = TextFont::Small,
                .color = look.full ? TICKET_BUY_KEY_COLOR : TICKET_RED,
                .alignment = TextAlignment::Center});
  if (look.chosen) {
    paintChoiceCorners(canvas, face);
  }
}

/** @brief Paints the two buttons, @p chosen the chosen one, then the stub
 * that the purchase tears off. */
void paintButtonsAndStub(IInterfaceCanvas& canvas, TicketButton chosen,
                         std::size_t item) {
  paintButton(canvas, {.area = TICKET_CANCEL_AREA,
                       .label = TICKET_CANCEL_LABEL,
                       .key = TICKET_CANCEL_KEY,
                       .full = false,
                       .chosen = chosen == TicketButton::Cancel});
  paintButton(canvas, {.area = TICKET_BUY_AREA,
                       .label = TICKET_BUY_LABEL,
                       .key = TICKET_BUY_KEY,
                       .full = true,
                       .chosen = chosen == TicketButton::Buy});
  const TextStyle stub{.font = TextFont::Small, .color = TICKET_RED};
  writeOnTicket(canvas, TICKET_CONTROL_LABEL,
                {TICKET_MARGIN, TICKET_CONTROL_ROW}, stub);
  writeOnTicket(canvas, priceTextOf(item),
                {TICKET_MARGIN, TICKET_STUB_PRICE_ROW},
                {.font = TextFont::Small, .color = TICKET_INK});
}

}  // namespace

void paintTicket(IInterfaceCanvas& canvas, const TicketView& view) {
  canvas.fill(INTERFACE_AREA, DIM_LAYER_COLOR);
  paintPaper(canvas, view.paid);
  paintForm(canvas);
  paintWare(canvas, view.item);
  paintWords(canvas, view.item);
  paintPrice(canvas, view.item);
  if (view.paid) {
    layPicture(canvas, TICKET_PAID_STAMP_RECTANGLE, TICKET_PAID_STAMP_TOP_LEFT);
    return;
  }
  paintButtonsAndStub(canvas, view.chosen, view.item);
}

}  // namespace rtype::client
