#pragma once

#include "ActionTimings.hpp"
#include "BehaviourView.hpp"
#include "Intent.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @returns @p intent asking to start the first phase of @p timings that lasts:
 * its signal, else its strike, else its recover.
 */
[[nodiscard]] Intent withStart(Intent intent, const ActionTimings& timings);

/**
 * @returns @p intent with the step that follows the phase of @p timings the
 * body of @p view is in, once that phase has lasted its time: the strike after
 * the signal, the recover after the strike, the end of the action after the
 * last phase that lasts. @p intent unchanged while the phase goes on, while
 * the body is idle, or in another attack.
 */
[[nodiscard]] Intent withNextPhase(Intent intent, const BehaviourView& view,
                                   const ActionTimings& timings);

/**
 * @returns Whether the body of @p view, idle, may start @p timings again: it
 * has been idle for @p rest at least, and long enough that its last start of
 * the same attack is ENEMY_ACTION_REPEAT_WINDOW behind, which the client
 * needs to tell a new action from a copy.
 */
[[nodiscard]] bool hasRested(const BehaviourView& view,
                             const ActionTimings& timings,
                             engine::Duration rest);

}  // namespace rtype::game
