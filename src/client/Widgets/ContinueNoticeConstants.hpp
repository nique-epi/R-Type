#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <chrono>
#include <string_view>
#include "MissionBannerConstants.hpp"
#include "TimeConstants.hpp"
#include "WarningBannerConstants.hpp"

namespace rtype::client {

/**
 * @brief The notice of a team sent back to a checkpoint: shown
 * CONTINUE_NOTICE_TIME from the status that told it, a band of
 * CONTINUE_NOTICE_BAND_COLOR across the playfield, CONTINUE_NOTICE_PADDING
 * rows above and under its line, ruled at its top and bottom row in the
 * orange of a warning, and its line centered on the middle of the playfield.
 */
constexpr engine::Duration CONTINUE_NOTICE_TIME = std::chrono::seconds(2);
constexpr int CONTINUE_NOTICE_PADDING = 3;
constexpr sf::Color CONTINUE_NOTICE_BAND_COLOR = WARNING_BAND_COLOR;

/** @brief Where the line of the notice is centered: the playfield above the
 * status band, as for the banner that opens a game. */
constexpr sf::IntRect CONTINUE_NOTICE_AREA = MISSION_BANNER_AREA;

/** @brief The words of the notice: CHECKPOINT, then the continues left by
 * their count, one, or none. */
constexpr std::string_view CONTINUE_NOTICE_OPENING = "CHECKPOINT - ";
constexpr std::string_view CONTINUES_LEFT_TEXT = " CONTINUES LEFT";
constexpr std::string_view ONE_CONTINUE_LEFT_TEXT = "1 CONTINUE LEFT";
constexpr std::string_view NO_CONTINUE_LEFT_TEXT = "NO CONTINUE LEFT";

}  // namespace rtype::client
