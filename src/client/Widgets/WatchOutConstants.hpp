#pragma once

#include <SFML/Graphics/Color.hpp>
#include <chrono>
#include <string_view>
#include "InterfaceColorConstants.hpp"
#include "TimeConstants.hpp"
#include "WarningBannerConstants.hpp"

namespace rtype::client {

/** @brief The warning of the charge of the boss: WATCH_OUT_TEXT over a
 * dashed lane on the row it charges along, for WATCH_OUT_TIME from the
 * signal of its charge; the text blinks every WATCH_OUT_BLINK_STEP, or stays
 * lit for a player who reduced flashes. */
constexpr std::string_view WATCH_OUT_TEXT = "WATCH OUT";
constexpr engine::Duration WATCH_OUT_TIME = std::chrono::milliseconds(1000);
constexpr engine::Duration WATCH_OUT_BLINK_STEP =
    std::chrono::milliseconds(150);

/** @brief The text sits WATCH_OUT_TEXT_LEFT pixels from the left edge, its
 * top WATCH_OUT_TEXT_RISE rows above the lane, as many more as its line grew
 * above the size the fonts are drawn at, never above the playfield. */
constexpr int WATCH_OUT_TEXT_LEFT = 6;
constexpr int WATCH_OUT_TEXT_RISE = 12;
constexpr sf::Color WATCH_OUT_TEXT_COLOR = WARNING_ORANGE;

/** @brief The lane: dashes WATCH_OUT_DASH pixels long every
 * WATCH_OUT_DASH_PERIOD, one row thick, scrolling left at
 * WATCH_OUT_SCROLL_PIXELS_PER_MILLISECOND, in HAZARD_RED at about 45 %. */
constexpr int WATCH_OUT_DASH = 6;
constexpr int WATCH_OUT_DASH_PERIOD = 10;
constexpr float WATCH_OUT_SCROLL_PIXELS_PER_MILLISECOND = 0.25F;
constexpr sf::Color WATCH_OUT_LANE_COLOR(HAZARD_RED.r, HAZARD_RED.g,
                                         HAZARD_RED.b, 115);

}  // namespace rtype::client
