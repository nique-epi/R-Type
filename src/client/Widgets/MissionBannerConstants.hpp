#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <chrono>
#include <string_view>
#include "InterfaceConstants.hpp"
#include "StatusBandConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The banner that opens a game: MISSION_BANNER_TEXT, written from
 * MISSION_BANNER_DELAY into the game, one more letter every
 * MISSION_BANNER_LETTER_TIME like the title of the title screen, then kept
 * whole MISSION_BANNER_HOLD.
 */
constexpr std::string_view MISSION_BANNER_TEXT = "MISSION START";
constexpr engine::Duration MISSION_BANNER_DELAY =
    std::chrono::milliseconds(1000);
constexpr engine::Duration MISSION_BANNER_LETTER_TIME =
    std::chrono::milliseconds(50);
constexpr engine::Duration MISSION_BANNER_HOLD =
    std::chrono::milliseconds(1000);

/** @brief Where the banner is centered: the playfield above the status band,
 * in pixels of the picture. */
constexpr sf::IntRect MISSION_BANNER_AREA(
    {0, 0}, {INTERFACE_AREA.size.x, INTERFACE_AREA.size.y - STATUS_BAND_ROWS});

/** @brief The rules above and under the line open from its middle, up to
 * MISSION_BANNER_RULE_HALF_WIDTH pixels on each side once every letter is
 * written, MISSION_BANNER_RULE_GAP pixels away from the line. */
constexpr int MISSION_BANNER_RULE_HALF_WIDTH = 64;
constexpr int MISSION_BANNER_RULE_GAP = 3;

}  // namespace rtype::client
