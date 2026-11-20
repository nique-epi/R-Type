#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include "BarState.hpp"
#include "ButlerLine.hpp"
#include "CasinoBar.hpp"
#include "CasinoSpriteConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief Where the butler stands: in front, on the right of the pavement,
 * his frame from CASINO_BUTLER_TOP_LEFT, its bottom cut by the edge of the
 * picture, so his size reads as the perspective of a palace seen from below.
 */
constexpr sf::Vector2i CASINO_BUTLER_TOP_LEFT(396, 176);

/**
 * @brief How the butler moves: an idle frame every BUTLER_IDLE_FRAME_TIME,
 * round and round; while the doors open, the four frames of his greeting then
 * the four of his bow, each OPENING_FRAME_TIME. The doors stay closed during
 * the greeting and open with the bow, and the hall opens once the bow
 * ended.
 */
constexpr engine::Duration BUTLER_IDLE_FRAME_TIME =
    std::chrono::milliseconds(250);
constexpr engine::Duration OPENING_FRAME_TIME = std::chrono::milliseconds(150);
constexpr int OPENING_POSE_COUNT = 2;
constexpr engine::Duration DOORS_OPEN_AFTER =
    OPENING_FRAME_TIME * BUTLER_FRAMES_PER_POSE;
constexpr engine::Duration OPENING_DURATION =
    DOORS_OPEN_AFTER * OPENING_POSE_COUNT;

/** @brief What the butler says before the doors, two lines short enough
 * for his panel. */
constexpr ButlerLine WELCOME_LINE{.first = "WELCOME TO THE BYDO PALACE,",
                                  .second = "PILOT. THE TABLES ARE OPEN."};

/** @brief The two bars before the doors, on the bottom edge of the panel of
 * the butler: ENTER, which opens them, and BACK, to the map. */
constexpr CasinoBar ENTER_BAR{.label = "ENTER",
                              .keyName = CONFIRM_KEY_NAME,
                              .area = sf::IntRect({172, 254}, {104, 14}),
                              .key = MENU_CONFIRM_KEY,
                              .restState = BarState::Selected};
constexpr CasinoBar DOORS_BACK_BAR{.label = BACK_LABEL,
                                   .keyName = CANCEL_KEY_NAME,
                                   .area = sf::IntRect({280, 254}, {88, 14}),
                                   .key = MENU_CANCEL_KEY};

}  // namespace rtype::client
