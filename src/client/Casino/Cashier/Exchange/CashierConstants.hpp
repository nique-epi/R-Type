#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <cstdint>
#include "BarState.hpp"
#include "CasinoBar.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief The amount moves by the value of the chip chosen in the selector,
 * one of CHIP_VALUES: the cashier opens on the chip of 100, the fourth, and
 * the amount 500.
 */
constexpr std::size_t CASHIER_FIRST_STEP = 3;
constexpr std::uint32_t CASHIER_FIRST_AMOUNT = 500;

/** @brief The keys that lower and raise the amount by the step. */
constexpr sf::Keyboard::Key CASHIER_LOWER_KEY = sf::Keyboard::Key::Left;
constexpr sf::Keyboard::Key CASHIER_RAISE_KEY = sf::Keyboard::Key::Right;

/**
 * @brief The key that switches the way of the cashier, buying chips or
 * cashing them out, and the one that picks the next chip as the step.
 */
constexpr sf::Keyboard::Key CASHIER_WAY_KEY = sf::Keyboard::Key::Tab;
constexpr const char* CASHIER_WAY_KEY_NAME = "TAB";
constexpr sf::Keyboard::Key CASHIER_NEXT_STEP_KEY = sf::Keyboard::Key::Up;

/**
 * @brief The exchange panel, on the right of the cage, and the name on its
 * tag, one per way. Every row of the panel shares its margins: what is on
 * the left starts at CASHIER_LEFT, what is on the right ends at
 * CASHIER_RIGHT, and the middle is CASHIER_MIDDLE.
 */
constexpr sf::IntRect CASHIER_PANEL_AREA({238, 126}, {236, 140});
constexpr int CASHIER_PANEL_MARGIN = 10;
constexpr int CASHIER_LEFT =
    CASHIER_PANEL_AREA.position.x + CASHIER_PANEL_MARGIN;
constexpr int CASHIER_RIGHT = CASHIER_PANEL_AREA.position.x +
                              CASHIER_PANEL_AREA.size.x - CASHIER_PANEL_MARGIN;
constexpr int CASHIER_MIDDLE = CASHIER_PANEL_AREA.position.x +
                               (CASHIER_PANEL_AREA.size.x / SIDES_PER_AXIS);
constexpr sf::Vector2i CASHIER_TAG_TOP_LEFT(CASHIER_LEFT, 120);
constexpr const char* BUY_CHIPS_TITLE = "BUY CHIPS";
constexpr const char* CASH_OUT_TITLE = "CASH OUT";

/**
 * @brief The balances the cashier changes, from the left, its source and its
 * destination: their names in small print, their amounts under them, the
 * marks between them in the middle; then the rule under them.
 */
constexpr int CASHIER_CAPTION_TOP = 136;
constexpr int CASHIER_BALANCE_TOP = 145;
constexpr const char* CASHIER_TOWARDS_MARK = ">>";
constexpr int CASHIER_TOWARDS_MARK_TOP = CASHIER_BALANCE_TOP;
constexpr sf::IntRect CASHIER_UPPER_RULE({CASHIER_LEFT, 157},
                                         {CASHIER_RIGHT - CASHIER_LEFT, 1});
constexpr sf::IntRect CASHIER_LOWER_RULE({CASHIER_LEFT, 212},
                                         {CASHIER_RIGHT - CASHIER_LEFT, 1});

/**
 * @brief The amount, in a box in the middle, 326 to 385, between two marks
 * the arrows move, 6 pixels from the box: "<" ends at 320, ">" starts at
 * 392, each inside a clickable area; its name on the left and the step on
 * the right, in small print one row under the text of the box.
 */
constexpr const char* CASHIER_AMOUNT_CAPTION = "AMOUNT";
constexpr const char* CASHIER_STEP_CAPTION = "STEP ";
constexpr sf::IntRect CASHIER_AMOUNT_BOX({326, 162}, {60, 16});
constexpr int CASHIER_AMOUNT_BORDER = 1;
constexpr int CASHIER_AMOUNT_TEXT_TOP = 166;
constexpr int CASHIER_AMOUNT_TOP = 167;
constexpr const char* CASHIER_LOWER_MARK = "<";
constexpr const char* CASHIER_RAISE_MARK = ">";
constexpr int CASHIER_LOWER_MARK_RIGHT = 320;
constexpr int CASHIER_RAISE_MARK_LEFT = 392;
constexpr sf::IntRect CASHIER_LOWER_MARK_AREA({310, 162}, {12, 16});
constexpr sf::IntRect CASHIER_RAISE_MARK_AREA({390, 162}, {12, 16});

/** @brief Where the first chip of the selector lies, its top-left corner:
 * the five chips side by side, CHIP_SELECTOR_STRIDE apart, centered on the
 * middle; the chosen one framed CASHIER_CHOSEN_CHIP_MARGIN around it, never
 * raised. */
constexpr sf::Vector2i CASHIER_FIRST_CHIP(297, 184);
constexpr int CASHIER_CHOSEN_CHIP_MARGIN = 1;

/**
 * @brief Under the lower rule: AFTER on the left and the rate on the right,
 * then what both balances hold after the exchange, or, in the middle, why it
 * is refused or that it is being made.
 */
constexpr int CASHIER_AFTER_TOP = 217;
constexpr int CASHIER_RESULT_TOP = 228;
constexpr int CASHIER_NOTICE_TOP = 229;
constexpr const char* CASHIER_AFTER_CAPTION = "AFTER";
constexpr const char* BUY_CHIPS_RATE = "1 KOPECK = 1 CHIP";
constexpr const char* CASH_OUT_RATE = "1 CHIP = 1 KOPECK";
constexpr const char* KOPECKS_WORD = "KOPECKS";
constexpr const char* CHIPS_WORD = "CHIPS";
constexpr const char* CASHIER_AMOUNT_WORD_GAP = " ";

/** @brief Why the cashier refuses: no amount, a balance short of it, after
 * NOT_ENOUGH_START, its name, MISSING_SEPARATOR and the number missing before
 * MISSING_END, or a balance that cannot hold more, after FULL_START. */
constexpr const char* NOTHING_TO_EXCHANGE_LINE = "CHOOSE AN AMOUNT";
/** @brief What the cashier says while the master reads the wallet or makes
 * the exchange, and when it refuses it. */
constexpr const char* READING_WALLET_LINE = "READING YOUR WALLET...";
constexpr const char* EXCHANGING_LINE = "CHANGING...";
constexpr const char* BALANCES_REFUSED_LINE = "THE BALANCES REFUSE IT";
constexpr const char* SIGN_IN_AGAIN_LINE = "SIGN IN AGAIN";
constexpr const char* MASTER_REFUSED_LINE = "THE MASTER REFUSED";
constexpr const char* MASTER_SILENT_LINE = "NO ANSWER FROM THE MASTER";
constexpr const char* NOT_ENOUGH_START = "NOT ENOUGH ";
constexpr const char* MISSING_SEPARATOR = ": ";
constexpr const char* MISSING_END = " MISSING";
constexpr const char* FULL_START = "NO ROOM LEFT FOR MORE ";

/** @brief The bars of the cashier, on the bottom row of the panel: the
 * exchange, named after its way, on the left, and BACK, to the hall, on the
 * right. */
constexpr sf::Vector2i CASHIER_BAR_SIZE(106, 14);
constexpr int CASHIER_BARS_TOP = 247;
constexpr sf::IntRect CASHIER_CONFIRM_AREA({CASHIER_LEFT, CASHIER_BARS_TOP},
                                           CASHIER_BAR_SIZE);
constexpr CasinoBar CASHIER_BACK_BAR{
    .label = BACK_LABEL,
    .keyName = CANCEL_KEY_NAME,
    .area = sf::IntRect({CASHIER_RIGHT - CASHIER_BAR_SIZE.x, CASHIER_BARS_TOP},
                        CASHIER_BAR_SIZE),
    .key = MENU_CANCEL_KEY};
constexpr CasinoBar BUY_CHIPS_CONFIRM_BAR{
    .label = {.english = "BUY", .french = "ACHAT"},
    .keyName = CONFIRM_KEY_NAME,
    .area = CASHIER_CONFIRM_AREA,
    .key = MENU_CONFIRM_KEY,
    .restState = BarState::Selected};
constexpr CasinoBar CASH_OUT_CONFIRM_BAR{
    .label = {.english = "CASH OUT", .french = "RETIRER"},
    .keyName = CONFIRM_KEY_NAME,
    .area = CASHIER_CONFIRM_AREA,
    .key = MENU_CONFIRM_KEY,
    .restState = BarState::Selected};
/** @brief The key that switches the way, never drawn: TAB. */
constexpr CasinoBar CASHIER_WAY_BAR{.label = {.english = "", .french = ""},
                                    .keyName = CASHIER_WAY_KEY_NAME,
                                    .area = sf::IntRect{},
                                    .key = CASHIER_WAY_KEY};

}  // namespace rtype::client
