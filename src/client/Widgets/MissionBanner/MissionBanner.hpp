#pragma once

#include <cstddef>
#include "IInterfaceCanvas.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How much of the banner shows: how many letters, and how far each
 * rule opened from the middle, in pixels. */
struct MissionBannerLook {
  std::size_t letters{};
  int ruleHalfWidth{};

  bool operator==(const MissionBannerLook&) const = default;
};

/**
 * @returns The banner at that time since the start of the game: nothing
 * before MISSION_BANNER_DELAY; then one more letter every
 * MISSION_BANNER_LETTER_TIME, the first at once, the rules opening with the
 * letters; whole for MISSION_BANNER_HOLD once every letter is written; then
 * nothing.
 */
[[nodiscard]] MissionBannerLook missionBannerAt(engine::Duration sinceStart);

/**
 * @brief Paints the banner centered in MISSION_BANNER_AREA: the letters shown
 * in HUD_CYAN outlined in INK, placed where the whole line goes so a new
 * letter never moves the others, between two rules of EDGE_BLUE. Nothing
 * when no letter shows.
 */
void paintMissionBanner(IInterfaceCanvas& canvas, MissionBannerLook look);

}  // namespace rtype::client
