#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cstddef>
#include "BackgroundColorConstants.hpp"
#include "CutCornerBox.hpp"
#include "DestinationCard.hpp"
#include "DestinationSpriteConstants.hpp"
#include "HangarScreenIdConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "KopecksLabelConstants.hpp"
#include "RenderingConstants.hpp"
#include "SectorMapSpriteConstants.hpp"
#include "ShopScreenIdConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief The title of the screen, on the top-left corner of the safe margin,
 * and its rule, which stops BUTTON_GAP short of the balance of kopecks. */
constexpr const char* DESTINATION_TITLE = "CHOOSE A DESTINATION";
constexpr sf::Vector2i DESTINATION_TITLE_ANCHOR(SAFE_MARGIN, SAFE_MARGIN);
constexpr TextStyle DESTINATION_TITLE_STYLE{.font = TextFont::Regular,
                                            .color = PAGE_TEXT_COLOR};
constexpr int DESTINATION_RULE_TOP = 38;
constexpr sf::IntRect DESTINATION_RULE({SAFE_MARGIN, DESTINATION_RULE_TOP},
                                       {KOPECKS_BALANCE_TOP_RIGHT.x -
                                            KOPECKS_FRAME_SIZE.x - BUTTON_GAP -
                                            SAFE_MARGIN,
                                        1});

/**
 * @brief The card in the middle, framed twice, and the two at its sides,
 * smaller, lower and dimmed, SIDE_CARD_GAP from it. The slots go from the
 * left: the card of slot CHOSEN_CARD_SLOT is the one ENTER opens.
 */
constexpr sf::Vector2i CHOSEN_CARD_SIZE(156, 164);
constexpr int CHOSEN_CARD_TOP = 48;
constexpr sf::Vector2i SIDE_CARD_SIZE(104, 120);
constexpr int SIDE_CARD_TOP = 70;
constexpr int SIDE_CARD_GAP = 20;
constexpr int CHOSEN_CARD_LEFT =
    (INTERFACE_AREA.size.x - CHOSEN_CARD_SIZE.x) / HALF_DIVISOR;
constexpr std::size_t CHOSEN_CARD_SLOT = 1;
constexpr std::array DESTINATION_CARD_AREAS{
    sf::IntRect(
        {CHOSEN_CARD_LEFT - SIDE_CARD_GAP - SIDE_CARD_SIZE.x, SIDE_CARD_TOP},
        SIDE_CARD_SIZE),
    sf::IntRect({CHOSEN_CARD_LEFT, CHOSEN_CARD_TOP}, CHOSEN_CARD_SIZE),
    sf::IntRect(
        {CHOSEN_CARD_LEFT + CHOSEN_CARD_SIZE.x + SIDE_CARD_GAP, SIDE_CARD_TOP},
        SIDE_CARD_SIZE)};

/** @brief The outer frame of the card in the middle, CHOSEN_FRAME_MARGIN
 * around it, then its own box; the box of a card at a side. */
constexpr int CHOSEN_FRAME_MARGIN = 2;
constexpr CutCornerBox::Colors CHOSEN_FRAME_COLORS{.border = EDGE_BLUE,
                                                   .inside = SKY_COLOR};
constexpr CutCornerBox::Colors CHOSEN_CARD_COLORS{
    .border = SELECTED_BAR_BORDER_COLOR, .inside = SKY_COLOR};
constexpr CutCornerBox::Colors SIDE_CARD_COLORS{.border = DEEP_BLUE,
                                                .inside = INK};

/** @brief Every sprite is enlarged by the whole factor that fits it in a box
 * of DESTINATION_SPRITE_BOX pixels, centered across its card, CHOSEN_SPRITE_TOP
 * or SIDE_SPRITE_TOP under the top of its card. The sprites at the sides are
 * dimmed by SIDE_SPRITE_TINT. */
constexpr int DESTINATION_SPRITE_BOX = 64;
constexpr int CHOSEN_SPRITE_TOP = 22;
constexpr int SIDE_SPRITE_TOP = 14;
constexpr sf::Color SIDE_SPRITE_TINT(255, 255, 255, 140);

/** @brief The name of the card in the middle, twice the size of a line,
 * CHOSEN_LABEL_TOP under its top; that of a card at a side,
 * SIDE_LABEL_FROM_BOTTOM above its bottom. */
constexpr int CHOSEN_LABEL_SCALE = 2;
constexpr int CHOSEN_LABEL_TOP = 104;
constexpr int SIDE_LABEL_FROM_BOTTOM = 24;
constexpr TextStyle CHOSEN_LABEL_STYLE{.font = TextFont::Regular,
                                       .color = STAR_WHITE,
                                       .outlined = true,
                                       .alignment = TextAlignment::Center,
                                       .scale = CHOSEN_LABEL_SCALE};
constexpr TextStyle SIDE_LABEL_STYLE{.font = TextFont::Regular,
                                     .color = PALE_BLUE,
                                     .alignment = TextAlignment::Center};

/** @brief The marks that say the cards turn, TURN_MARK_GAP outside the cards
 * at the sides, level with the middle of the card in the middle. */
constexpr const char* TURN_LEFT_MARK = "<";
constexpr const char* TURN_RIGHT_MARK = ">";
constexpr int TURN_MARK_GAP = 10;
constexpr int TURN_MARK_TOP =
    CHOSEN_CARD_TOP + ((CHOSEN_CARD_SIZE.y - TEXT_LINE_HEIGHT) / HALF_DIVISOR);
constexpr TextStyle TURN_MARK_STYLE{.font = TextFont::Regular,
                                    .color = HUD_CYAN,
                                    .alignment = TextAlignment::Center};
constexpr sf::Keyboard::Key TURN_LEFT_KEY = sf::Keyboard::Key::Left;
constexpr sf::Keyboard::Key TURN_RIGHT_KEY = sf::Keyboard::Key::Right;

/** @brief The description of the card in the middle, centered under the
 * cards, as the description of a menu. */
constexpr sf::Vector2i DESTINATION_DESCRIPTION_ANCHOR(INTERFACE_AREA.size.x /
                                                          HALF_DIVISOR,
                                                      222);
constexpr TextStyle DESTINATION_DESCRIPTION_STYLE{
    .font = TextFont::Small,
    .color = FORM_DESCRIPTION_COLOR,
    .alignment = TextAlignment::Center};

/** @brief The chip that opens the card in the middle, beside BACK. */
constexpr const char* GO_LABEL = "GO";

/** @brief The cards, from the left as the screen opens: SERVER in the
 * middle. SERVER leads where PLAY led, so it is built with the start of the
 * client (see serverCardOf()). */
constexpr const char* SERVER_LABEL = "SERVER";
constexpr const char* SERVER_DESCRIPTION =
    "JOIN A SECTOR AND FLY WITH UP TO 4 PILOTS";
constexpr DestinationCard SHOP_CARD{
    .label = SHOP_LABEL,
    .description = "SPEND YOUR KOPECKS ON HATS AND SHIPS",
    .sheet = DESTINATION_SHEET_ID,
    .frame = DESTINATION_BASKET_FRAME,
    .screen = SHOP_SCREEN};
constexpr DestinationCard HANGAR_CARD{
    .label = HANGAR_LABEL,
    .description = "DRESS YOUR SHIP FROM YOUR LOCKER",
    .sheet = SECTOR_MAP_SHEET_ID,
    .frame = HANGAR_HOUSE_FRAME,
    .screen = HANGAR_SCREEN};
constexpr std::size_t DESTINATION_COUNT = DESTINATION_CARD_AREAS.size();

}  // namespace rtype::client
