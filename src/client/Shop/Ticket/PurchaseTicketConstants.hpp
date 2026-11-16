#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <string_view>
#include "InterfaceConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The sales slip of a ware, centred on the interface picture: its
 * paper, its stub under the perforation included. Every other place of the
 * ticket is counted from its top-left corner. */
constexpr sf::Vector2i TICKET_SIZE{172, 216};
constexpr sf::IntRect TICKET_AREA{(INTERFACE_AREA.size - TICKET_SIZE) / 2,
                                  TICKET_SIZE};

/** @brief Where the pictures of the ticket lie on SHOP_TICKET_SHEET_ID: its
 * paper and the shadow it casts, the bottom edge left once its stub is torn
 * off, laid TICKET_TORN_EDGE_TOP rows down the ticket, and its PAID stamp. */
constexpr sf::IntRect TICKET_PAPER_RECTANGLE{{0, 0}, {175, 219}};
constexpr sf::IntRect TICKET_TORN_EDGE_RECTANGLE{{0, 224}, {175, 8}};
constexpr sf::IntRect TICKET_PAID_STAMP_RECTANGLE{{0, 236}, {88, 34}};
constexpr int TICKET_TORN_EDGE_TOP = 186;
constexpr sf::Vector2i TICKET_PAID_STAMP_TOP_LEFT{42, 153};

/** @brief How long the ticket shows its PAID stamp before it goes. */
constexpr engine::Duration TICKET_PAID_TIME = std::chrono::milliseconds(700);

/** @brief The text of the ticket stands TICKET_MARGIN in from its sides, as
 * wide as TICKET_TEXT_WIDTH: nineteen letters of the regular font. */
constexpr int TICKET_MARGIN = 10;
constexpr int TICKET_TEXT_WIDTH = TICKET_SIZE.x - (2 * TICKET_MARGIN);
constexpr int TICKET_MIDDLE = TICKET_SIZE.x / 2;

/** @brief The red rules printed on the form: a double one at its top, one
 * under its heading, a dotted one over the price and one under it. */
constexpr int TICKET_RULE_LEFT = 8;
constexpr int TICKET_RULE_WIDTH = TICKET_SIZE.x - (2 * TICKET_RULE_LEFT);
constexpr int TICKET_TOP_RULE_ROW = 6;
constexpr int TICKET_SECOND_TOP_RULE_ROW = 8;
constexpr int TICKET_HEADING_RULE_ROW = 37;
constexpr int TICKET_DOTTED_RULE_ROW = 132;
constexpr int TICKET_PRICE_RULE_ROW = 156;
constexpr int TICKET_DOT_PITCH = 2;

/** @brief The rows of the lines of text, each the top of its line. */
constexpr int TICKET_FORM_ROW = 12;
constexpr int TICKET_HEADING_ROW = 24;
constexpr int TICKET_NAME_ROW = 88;
constexpr int TICKET_DESCRIPTION_ROW = 100;
constexpr int TICKET_DESCRIPTION_LINE_HEIGHT = 10;
constexpr int TICKET_LABEL_ROW = 137;
constexpr int TICKET_PRICE_ROW = 145;
constexpr int TICKET_CONTROL_ROW = 194;
constexpr int TICKET_STUB_PRICE_ROW = 204;
constexpr int TICKET_PRICE_LABEL_LEFT = 104;

/** @brief The ware is drawn as large as it fits in this area, centred on
 * the middle of the ticket, over its name. */
constexpr sf::IntRect TICKET_WARE_AREA{{TICKET_MIDDLE - 36, 42}, {72, 40}};

/** @brief The two buttons of the ticket, and the label and key of each: the
 * key centred under the label: the letters of the label on rows 4 to 10 of
 * the face, those of the key on rows 14 to 18, two rows clear of the border
 * on row 21. */
constexpr sf::IntRect TICKET_CANCEL_AREA{{12, 160}, {66, 22}};
constexpr sf::IntRect TICKET_BUY_AREA{{80, 160}, {80, 22}};
constexpr int TICKET_BUTTON_LABEL_ROW = 3;
constexpr int TICKET_BUTTON_KEY_ROW = 12;
/** @brief A chosen button sinks by one row, and shows the corners of the
 * choice TICKET_CORNER_GAP around it, each TICKET_CORNER_LENGTH long. */
constexpr int TICKET_BUTTON_SINK = 1;
constexpr int TICKET_BUTTON_SHADOW = 2;
constexpr int TICKET_CORNER_GAP = 2;
constexpr int TICKET_CORNER_LENGTH = 4;

/** @brief The inks of the ticket: the red of the form, its dark red, the
 * ink of the typed text, and the faces of the buttons, lighter once
 * chosen. */
constexpr sf::Color TICKET_RED(180, 48, 42);
constexpr sf::Color TICKET_DARK_RED(126, 31, 28);
constexpr sf::Color TICKET_INK(43, 38, 64);
constexpr sf::Color TICKET_BUY_FACE(192, 57, 47);
constexpr sf::Color TICKET_CHOSEN_BUY_FACE(217, 88, 74);
constexpr sf::Color TICKET_BUY_KEY_COLOR(246, 214, 200);
constexpr sf::Color TICKET_CANCEL_FACE(232, 214, 166);
constexpr sf::Color TICKET_CHOSEN_CANCEL_FACE(243, 230, 196);
constexpr sf::Color TICKET_BUTTON_LIGHT(255, 255, 255, 110);

constexpr std::string_view TICKET_STORE_LABEL = "STORE No 7";
constexpr std::string_view TICKET_SERIES_LABEL = "СЕРИЯ АБ 011";
constexpr std::string_view TICKET_HEADING = "SALES SLIP";
constexpr std::string_view TICKET_QUANTITY_LABEL = "QTY 1";
constexpr std::string_view TICKET_PRICE_LABEL = "PRICE";
constexpr std::string_view TICKET_PRICE_SUFFIX = " K.";
constexpr std::string_view TICKET_CONTROL_LABEL = "CONTROL";
constexpr std::string_view TICKET_CANCEL_LABEL = "CANCEL";
constexpr std::string_view TICKET_CANCEL_KEY = "[ESC]";
constexpr std::string_view TICKET_BUY_LABEL = "BUY";
constexpr std::string_view TICKET_BUY_KEY = "[ENTER]";

}  // namespace rtype::client
