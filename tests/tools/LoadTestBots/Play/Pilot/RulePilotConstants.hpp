#pragma once

#include <cstddef>

namespace rtype::tools {

/** @brief How far ahead a pilot checks what will touch its ship: six steps of
 * a tenth of a second, 288 units at the speed of a ship. */
constexpr std::size_t RULE_LOOK_AHEAD_STEPS = 6;
constexpr float RULE_LOOK_AHEAD_STEP = 0.1F;

/** @brief The room kept around the ship, in logical units: the inputs and the
 * world states lag behind what the server plays. */
constexpr float RULE_DANGER_MARGIN = 24.0F;

/** @brief What a touch costs, per step left before the end of the look ahead:
 * far more than any distance, so a safe move always wins. */
constexpr float RULE_TOUCH_COST = 100000.0F;

/** @brief How far from the row of its target a ship may stay and still hit it:
 * half the 96 units of the hitbox of a drifter. */
constexpr float RULE_ALIGN_TOLERANCE = 48.0F;

/** @brief Where a ship waits, near the left edge, so it sees what comes from
 * the right early, and what straying from there costs per unit. */
constexpr float RULE_HOME_X = 300.0F;
constexpr float RULE_HOME_COST = 0.25F;

/** @brief The width from which an enemy is a boss: 96 for the others, 440 for
 * the tyrant. */
constexpr float RULE_BOSS_SMALLEST_WIDTH = 300.0F;

}  // namespace rtype::tools
