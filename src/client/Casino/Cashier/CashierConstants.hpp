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

/** @brief The panel of the cashier over the dimmed hall, and the name on its
 * tag, one per way. */
constexpr sf::IntRect CASHIER_PANEL_AREA({84, 48}, {312, 182});
constexpr sf::Vector2i CASHIER_TAG_TOP_LEFT(94, 42);
constexpr const char* BUY_CHIPS_TITLE = "BUY CHIPS";
constexpr const char* CASH_OUT_TITLE = "CASH OUT";

/**
 * @brief The balances the cashier changes, from the left, its source and its
 * destination: their names in small print, their amounts under them, the
 * marks between them in the middle; then the rule under them. Every column
 * of the panel lies between CASHIER_LEFT and CASHIER_RIGHT.
 */
constexpr int CASHIER_LEFT = 98;
constexpr int CASHIER_RIGHT = 382;
constexpr int CASHIER_MIDDLE = 240;
constexpr int CASHIER_CAPTION_TOP = 62;
constexpr int CASHIER_BALANCE_TOP = 72;
constexpr const char* CASHIER_TOWARDS_MARK = ">>";
constexpr int CASHIER_TOWARDS_MARK_TOP = 70;
constexpr sf::IntRect CASHIER_UPPER_RULE({96, 86}, {288, 1});
constexpr sf::IntRect CASHIER_LOWER_RULE({96, 166}, {288, 1});

/**
 * @brief The amount, in a box between two marks the arrows move, its name on
 * the left, the step on the right.
 */
constexpr int CASHIER_AMOUNT_TOP = 98;
constexpr const char* CASHIER_AMOUNT_CAPTION = "AMOUNT";
constexpr const char* CASHIER_STEP_CAPTION = "STEP ";
constexpr sf::IntRect CASHIER_AMOUNT_BOX({210, 93}, {60, 16});
constexpr int CASHIER_AMOUNT_BORDER = 1;
constexpr int CASHIER_AMOUNT_TEXT_TOP = 97;
constexpr const char* CASHIER_LOWER_MARK = "<";
constexpr const char* CASHIER_RAISE_MARK = ">";
constexpr sf::IntRect CASHIER_LOWER_MARK_AREA({192, 93}, {12, 16});
constexpr sf::IntRect CASHIER_RAISE_MARK_AREA({276, 93}, {12, 16});

/** @brief Where the first chip of the selector lies at rest, its top-left
 * corner. */
constexpr sf::Vector2i CASHIER_FIRST_CHIP(168, 120);

/**
 * @brief Under the lower rule: AFTER, the rate in the middle, then what both
 * balances hold after the exchange, or why it is refused.
 */
constexpr int CASHIER_AFTER_TOP = 172;
constexpr int CASHIER_RESULT_TOP = 182;
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

/** @brief The two bars of the cashier: the exchange, named after its way, and
 * BACK, to the hall. */
constexpr sf::IntRect CASHIER_CONFIRM_AREA({98, 204}, {130, 14});
constexpr CasinoBar CASHIER_BACK_BAR{.label = BACK_LABEL,
                                     .keyName = CANCEL_KEY_NAME,
                                     .area = sf::IntRect({252, 204}, {130, 14}),
                                     .key = MENU_CANCEL_KEY};
constexpr CasinoBar BUY_CHIPS_CONFIRM_BAR{.label = "BUY",
                                          .keyName = CONFIRM_KEY_NAME,
                                          .area = CASHIER_CONFIRM_AREA,
                                          .key = MENU_CONFIRM_KEY,
                                          .restState = BarState::Selected};
constexpr CasinoBar CASH_OUT_CONFIRM_BAR{.label = "CASH OUT",
                                         .keyName = CONFIRM_KEY_NAME,
                                         .area = CASHIER_CONFIRM_AREA,
                                         .key = MENU_CONFIRM_KEY,
                                         .restState = BarState::Selected};

}  // namespace rtype::client
