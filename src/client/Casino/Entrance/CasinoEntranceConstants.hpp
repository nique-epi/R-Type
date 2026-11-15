#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include "BarState.hpp"
#include "ButlerLine.hpp"
#include "CasinoBar.hpp"
#include "CasinoSpriteConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief Where the butler stands: his frame CASINO_BUTLER_TOP from the top of
 * the picture, so his feet rest on the top step; beside the doors before they
 * open, then aside in the hall, to leave room for its menu.
 */
constexpr int CASINO_BUTLER_TOP = 72;
constexpr int BUTLER_AT_THE_DOORS_LEFT = 170;
constexpr int BUTLER_IN_THE_HALL_LEFT = 110;

/**
 * @brief How the butler moves: an idle frame every BUTLER_IDLE_FRAME_TIME,
 * round and round; while the doors open, the four frames of his greeting then
 * the four of his bow, each OPENING_FRAME_TIME. The doors stand ajar during
 * the greeting and open with the bow, and the hall shows once the bow ended.
 */
constexpr engine::Duration BUTLER_IDLE_FRAME_TIME =
    std::chrono::milliseconds(250);
constexpr engine::Duration OPENING_FRAME_TIME = std::chrono::milliseconds(150);
constexpr int OPENING_POSE_COUNT = 2;
constexpr engine::Duration DOORS_OPEN_AFTER =
    OPENING_FRAME_TIME * BUTLER_FRAMES_PER_POSE;
constexpr engine::Duration OPENING_DURATION =
    DOORS_OPEN_AFTER * OPENING_POSE_COUNT;

/** @brief What the butler says before the doors, then in the hall. */
constexpr ButlerLine WELCOME_LINE{.first = "WELCOME TO THE BYDO PALACE, PILOT.",
                                  .second = "THE TABLES ARE OPEN TONIGHT."};
constexpr ButlerLine HALL_LINE{.first = "WHAT SHALL IT BE TONIGHT, PILOT?",
                               .second = "THE CASHIER CHANGES BOTH WAYS."};

/** @brief The two bars before the doors, on the bottom edge of the panel of
 * the butler: ENTER, which opens them, and BACK, to the waiting room. */
constexpr CasinoBar ENTER_BAR{.label = "ENTER",
                              .keyName = CONFIRM_KEY_NAME,
                              .area = sf::IntRect({216, 254}, {104, 14}),
                              .key = MENU_CONFIRM_KEY,
                              .restState = BarState::Selected};
constexpr CasinoBar DOORS_BACK_BAR{.label = BACK_LABEL,
                                   .keyName = CANCEL_KEY_NAME,
                                   .area = sf::IntRect({324, 254}, {88, 14}),
                                   .key = MENU_CANCEL_KEY};

/**
 * @brief The menu of the hall: a gold panel named by its tag, and its bars,
 * HALL_BAR_SPACING apart from the top of one to the top of the next, LEAVE
 * HALL_LEAVE_DROP lower again.
 */
constexpr sf::IntRect HALL_PANEL_AREA({276, 74}, {186, 106});
constexpr const char* HALL_TAG = "HALL";
constexpr sf::Vector2i HALL_TAG_TOP_LEFT(286, 68);
constexpr int HALL_BARS_LEFT = 286;
constexpr int HALL_FIRST_BAR_TOP = 88;
constexpr sf::Vector2i HALL_BAR_SIZE(166, 14);
constexpr int HALL_BAR_SPACING = 20;
constexpr int HALL_LEAVE_DROP = 4;

/** @brief The bars of the hall, from the top. */
constexpr CasinoBar ROULETTE_BAR{
    .label = "ROULETTE",
    .keyName = CONFIRM_KEY_NAME,
    .area = sf::IntRect({HALL_BARS_LEFT, HALL_FIRST_BAR_TOP}, HALL_BAR_SIZE),
    .key = MENU_CONFIRM_KEY,
    .restState = BarState::Selected};
constexpr CasinoBar BUY_CHIPS_BAR{
    .label = "BUY CHIPS",
    .keyName = "B",
    .area = sf::IntRect({HALL_BARS_LEFT, HALL_FIRST_BAR_TOP + HALL_BAR_SPACING},
                        HALL_BAR_SIZE),
    .key = sf::Keyboard::Key::B};
constexpr CasinoBar CASH_OUT_BAR{
    .label = "CASH OUT",
    .keyName = "C",
    .area = sf::IntRect(
        {HALL_BARS_LEFT, HALL_FIRST_BAR_TOP + (HALL_BAR_SPACING * 2)},
        HALL_BAR_SIZE),
    .key = sf::Keyboard::Key::C};
constexpr CasinoBar LEAVE_BAR{
    .label = "LEAVE",
    .keyName = CANCEL_KEY_NAME,
    .area = sf::IntRect(
        {HALL_BARS_LEFT,
         HALL_FIRST_BAR_TOP + (HALL_BAR_SPACING * 3) + HALL_LEAVE_DROP},
        HALL_BAR_SIZE),
    .key = MENU_CANCEL_KEY};

}  // namespace rtype::client
