#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "TextMetrics.hpp"
#include "TimeConstants.hpp"

/**
 * @brief The boss of the bar tests: 240 points, each gauge holding 120 of
 * them on its 98 inner columns. 230 points light ceil(98 × 110 / 120) = 90
 * columns of gauge I; 119 points leave gauge I empty; 1 point still lights a
 * column of gauge II.
 */
constexpr std::uint16_t BAR_FULL_HEALTH = 240;
constexpr std::uint16_t BAR_AFTER_ONE_HIT = 230;
constexpr std::uint16_t BAR_BELOW_HALF = 119;
constexpr std::uint16_t BAR_LAST_POINT = 1;
constexpr int GAUGE_INNER_COLUMNS = 98;

/** @brief The largest text size, at which the name of the boss is 16 rows
 * tall over a bar 16 rows from the top. */
constexpr rtype::client::TextMetrics LARGEST_BAR_TEXT{.halfPixelsPerFontPixel =
                                                          4};
constexpr int COLUMNS_OF_110_POINTS_OF_120 = 90;
constexpr int ONE_COLUMN = 1;

/** @brief The times of the bar tests, the fight starting at 0. */
constexpr rtype::engine::Duration FIGHT_START = rtype::engine::Duration::zero();
constexpr rtype::engine::Duration BAR_FILLED = std::chrono::milliseconds(800);
constexpr rtype::engine::Duration A_HIT_AT_ONE_SECOND = std::chrono::seconds(1);
constexpr rtype::engine::Duration JUST_AFTER_THE_HIT =
    std::chrono::milliseconds(1100);
constexpr rtype::engine::Duration HALF_A_SECOND_AFTER_THE_HIT =
    std::chrono::milliseconds(1500);
constexpr rtype::engine::Duration GAUGE_ONE_EMPTIED = std::chrono::seconds(2);
constexpr rtype::engine::Duration INTO_THE_SHATTER =
    std::chrono::milliseconds(2100);
constexpr int MILLISECONDS_INTO_THE_SHATTER = 100;
constexpr rtype::engine::Duration SHATTER_OVER =
    std::chrono::milliseconds(2500);

/** @brief The rage flickers every 80 ms: 800 ms is step 10, 879 ms still is,
 * 880 ms is step 11. */
constexpr rtype::engine::Duration FLICKER_STEP_TEN_START =
    std::chrono::milliseconds(800);
constexpr rtype::engine::Duration FLICKER_STEP_TEN_END =
    std::chrono::milliseconds(879);
constexpr rtype::engine::Duration FLICKER_STEP_ELEVEN_START =
    std::chrono::milliseconds(880);
constexpr int FLICKER_STEP_TEN = 10;
constexpr int FLICKER_STEP_ELEVEN = 11;

/** @brief Where the design lays the bar: the gauge of phase II from x 139,
 * the gauge of phase I from x 242, both 100 by 5 from y 16, so the bar runs
 * from x 139 to 341; the name centred on x 240, its top on y 6. */
constexpr sf::IntRect DESIGNED_PHASE_TWO_GAUGE({139, 16}, {100, 5});
constexpr sf::IntRect DESIGNED_PHASE_ONE_GAUGE({242, 16}, {100, 5});
constexpr std::string_view DESIGNED_BOSS_NAME = "BYDO TYRANT";
constexpr sf::Vector2i DESIGNED_NAME_ANCHOR(240, 6);
