#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <string_view>
#include "GameNoticeConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief The bar of the boss, at the top centre of the playfield: two
 * gauges of BOSS_GAUGE_WIDTH × BOSS_GAUGE_HEIGHT, phase II on the left and
 * phase I on the right, BOSS_GAUGE_GAP apart, the pair centred on
 * BOSS_BAR_CENTER_X with its top on BOSS_BAR_TOP: from x 139 to 341. */
constexpr int BOSS_GAUGE_WIDTH = 100;
constexpr int BOSS_GAUGE_HEIGHT = 5;
constexpr int BOSS_GAUGE_GAP = 3;
constexpr int BOSS_GAUGE_COUNT = 2;
constexpr int BOSS_BAR_WIDTH =
    (BOSS_GAUGE_COUNT * BOSS_GAUGE_WIDTH) + BOSS_GAUGE_GAP;
constexpr int BOSS_BAR_CENTER_X = INTERFACE_AREA.size.x / 2;
constexpr int BOSS_BAR_TOP = 16;
constexpr int BOSS_BAR_LEFT = BOSS_BAR_CENTER_X - (BOSS_BAR_WIDTH / 2);
constexpr sf::IntRect BOSS_PHASE_TWO_GAUGE({BOSS_BAR_LEFT, BOSS_BAR_TOP},
                                           {BOSS_GAUGE_WIDTH,
                                            BOSS_GAUGE_HEIGHT});
constexpr sf::IntRect BOSS_PHASE_ONE_GAUGE(
    {BOSS_BAR_LEFT + BOSS_GAUGE_WIDTH + BOSS_GAUGE_GAP, BOSS_BAR_TOP},
    {BOSS_GAUGE_WIDTH, BOSS_GAUGE_HEIGHT});

/** @brief A gauge is a border of BOSS_GAUGE_BORDER pixel around
 * BOSS_GAUGE_INNER_WIDTH columns of fill. */
constexpr int BOSS_GAUGE_BORDER = 1;
constexpr int BOSS_GAUGE_INNER_WIDTH =
    BOSS_GAUGE_WIDTH - (SIDES_PER_AXIS * BOSS_GAUGE_BORDER);

/** @brief The name of the boss, centred BOSS_NAME_GAP above the bar, and the
 * label of each gauge, centred BOSS_LABEL_GAP under it. */
constexpr std::string_view BOSS_NAME_TEXT = "BYDO TYRANT";
constexpr int BOSS_NAME_GAP = 2;
constexpr int BOSS_NAME_TOP = BOSS_BAR_TOP - BOSS_NAME_GAP - TEXT_LINE_HEIGHT;
constexpr std::string_view PHASE_ONE_LABEL = "I";
constexpr std::string_view PHASE_TWO_LABEL = "II";
constexpr int BOSS_LABEL_GAP = 2;
constexpr int BOSS_LABEL_TOP =
    BOSS_BAR_TOP + BOSS_GAUGE_HEIGHT + BOSS_LABEL_GAP;

/** @brief While the bar shows, the feed of departures starts under its
 * labels, NOTICE_EDGE lower, so a long name never runs under the bar. */
constexpr sf::Vector2i DEPARTURE_FEED_UNDER_BOSS_BAR(
    DEPARTURE_FEED_TOP_LEFT.x, BOSS_LABEL_TOP + TEXT_LINE_HEIGHT + NOTICE_EDGE);

/** @brief The bar fills from nothing in BOSS_BAR_FILL_TIME; the rage
 * flickers BOSS_HOT_PIXELS hot pixels, new ones every BOSS_BAR_FLICKER_STEP;
 * the emptied gauge I shatters into BOSS_GAUGE_SHARDS pixels falling
 * BOSS_SHARD_FALL rows in BOSS_GAUGE_SHATTER_TIME, drifting up to
 * BOSS_SHARD_DRIFT_PIXELS sideways; a spent gauge is hatched every
 * BOSS_HATCH_PERIOD pixels. */
constexpr engine::Duration BOSS_BAR_FILL_TIME = std::chrono::milliseconds(800);
constexpr engine::Duration BOSS_BAR_FLICKER_STEP =
    std::chrono::milliseconds(80);
constexpr engine::Duration BOSS_GAUGE_SHATTER_TIME =
    std::chrono::milliseconds(500);
constexpr int BOSS_HOT_PIXELS = 6;
constexpr int BOSS_HOT_STEP_STRIDE = 37;
constexpr int BOSS_HOT_PIXEL_STRIDE = 17;
constexpr int BOSS_GAUGE_SHARDS = 34;
constexpr int BOSS_SHARD_FALL = 40;
constexpr int BOSS_SHARD_DRIFTS = 5;
constexpr float BOSS_SHARD_DRIFT_PIXELS = 3.0F;
constexpr int BOSS_HATCH_PERIOD = 3;

}  // namespace rtype::client
