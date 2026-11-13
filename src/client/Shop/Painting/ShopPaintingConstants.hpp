#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "InterfaceConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief A cell of the locker of the shop: the icon of its look, in a
 * square SHOP_ICON_SIZE large SHOP_ICON_OFFSET inside the cell, standing on
 * its shelf; its price tag ends SHOP_CELL_TAG_INSET from the right side of
 * the cell, SHOP_CELL_TAG_TOP under its top. */
constexpr sf::Vector2i SHOP_ICON_OFFSET{8, 12};
constexpr int SHOP_ICON_SIZE = 16;
constexpr int SHOP_CELL_TAG_INSET = 3;
constexpr int SHOP_CELL_TAG_TOP = 2;

/** @brief A stall of the glass case: the ship SHOP_STALL_SHIP_TOP under its
 * top, its price tag centred SHOP_STALL_TAG_TOP under its top. */
constexpr int SHOP_STALL_SHIP_TOP = 4;
constexpr int SHOP_STALL_TAG_TOP = 26;

/** @brief The frame of the chosen ware, SHOP_CHOSEN_FRAME_GAP around it, in
 * the cyan of every selection, and the red ribbon across a ware the player
 * owns, SHOP_RIBBON_THICKNESS thick, rising SHOP_RIBBON_RISE rows every
 * SHOP_RIBBON_RUN columns. */
constexpr int SHOP_CHOSEN_FRAME_GAP = 2;
constexpr sf::Color SHOP_RIBBON_COLOR(196, 48, 43);
constexpr int SHOP_RIBBON_THICKNESS = 3;
constexpr int SHOP_RIBBON_INSET = 2;
constexpr int SHOP_RIBBON_RISE = 7;
constexpr int SHOP_RIBBON_RUN = 10;

/** @brief The shopkeeper: where his frame is laid at rest, the size of a
 * frame of his sheet, and how far apart its frames are, one per
 * ShopkeeperEye, in a row. */
constexpr sf::Vector2i SHOPKEEPER_TOP_LEFT{193, 85};
constexpr sf::Vector2i SHOPKEEPER_FRAME_SIZE{93, 104};
constexpr int SHOPKEEPER_FRAME_PITCH = 95;

/** @brief The radio of the counter, as the picture of the booth draws it: a
 * click on it changes the music, and the frame of a choice shows around it
 * under the pointer. */
constexpr sf::IntRect SHOP_RADIO_AREA{{280, 161}, {42, 21}};

/** @brief The balance of the player, ending in the top-right corner of the
 * shop, and the status line, centred at the bottom, both outlined in ink
 * over the picture. */
constexpr sf::Vector2i SHOP_BALANCE_TOP_RIGHT(INTERFACE_AREA.size.x -
                                                  SAFE_MARGIN,
                                              8);
constexpr sf::Vector2i SHOP_STATUS_ANCHOR(INTERFACE_AREA.size.x /
                                              SIDES_PER_AXIS,
                                          259);
constexpr TextStyle SHOP_STATUS_STYLE{.font = TextFont::Small,
                                      .color = STATUS_TEXT_COLOR,
                                      .outlined = true,
                                      .alignment = TextAlignment::Center};

}  // namespace rtype::client
