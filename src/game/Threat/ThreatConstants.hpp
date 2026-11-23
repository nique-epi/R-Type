#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief How far from the heading of its ship a projectile may fly and
 * still count in the firepower of its gun, in degrees: a projectile that
 * flies wider hits what comes from above or below, not what the ship faces.
 * The yellow weapon fires at 80 degrees up and down, which it leaves out, and
 * at 40 degrees on its second level, which it keeps. */
constexpr float FORWARD_CONE_DEGREES = 45.0F;

/** @brief How long the threat of a team takes to rise from none to the most,
 * when its firepower jumps from the guns of its sheets to the strongest
 * weapon: a ship that picks up a crystal is not met at once by the enemies
 * it faces. A threat falls at once, when a ship dies or leaves. */
constexpr engine::Duration THREAT_RISE = std::chrono::seconds(8);
constexpr std::size_t THREAT_RISE_TICKS = engine::wholeTicksIn(THREAT_RISE);

/** @brief The threat, in percent of the most, each tier above the first
 * starts at: tier 1 from 25 %, 2 from 50 %, 3 from 75 %. */
constexpr int THREAT_PERCENT_PER_TIER = 25;
constexpr int HIGHEST_THREAT_TIER = 3;

}  // namespace rtype::game
