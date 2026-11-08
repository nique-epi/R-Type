#pragma once

#include <span>
#include <string>
#include "BehaviourView.hpp"
#include "Intent.hpp"

struct lua_State;

namespace rtype::game {

/**
 * @brief Pushes on @p state the table a script reads of its body: points as
 * `{x, y}` in logical units, times in milliseconds, the values of the settings
 * by their @p settingNames, the action as `{attack, phase}` with the phase
 * named as in PHASE_NAMES, `health` the share of its health left, 0 to 1,
 * `enraged` whether it is a boss that enraged, `actions_ended` how many
 * actions it ended, `nearest_ship` and `action` absent when there is none.
 */
void pushBehaviourView(lua_State* state, const BehaviourView& view,
                       std::span<const std::string> settingNames);

/**
 * @returns The intent the table at @p index of @p state describes. A field
 * left out wants nothing; a direction is cut to -1..1 on each axis; an action
 * whose attack is not 0 to HIGHEST_ATTACK_NUMBER or whose phase is not one of
 * PHASE_NAMES is left out, as a collision box or a top speed that is not a
 * positive finite number; a value of the wrong type counts as left out.
 */
[[nodiscard]] Intent intentAt(lua_State* state, int index);

}  // namespace rtype::game
