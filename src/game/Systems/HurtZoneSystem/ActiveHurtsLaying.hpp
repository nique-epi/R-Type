#pragma once

#include "ActiveHurts.hpp"
#include "ContactShape.hpp"
#include "HurtingEnemy.hpp"

namespace rtype::game {

/**
 * @brief Empties @p hurts, then lays in it the boxes and the beams of the
 * HurtZone of @p enemy that hurt now: a part with no `activeIn` always, a part
 * that names an action only while the CurrentAction of the enemy is that
 * action and inside its window. A beam turns from fromDegrees to toDegrees
 * over its window. Its vectors keep their room, so a caller that keeps one
 * ActiveHurts allocates nothing once warm.
 */
void layActiveHurts(const HurtingEnemy& enemy, ActiveHurts& hurts);

/** @returns Whether an active box or an active beam of @p hurts touches
 * @p ship: its box, then its mask when it has one (shapesTouch(),
 * beamTouchesShape()). */
[[nodiscard]] bool hurtsShape(const ActiveHurts& hurts,
                              const ContactShape& ship);

}  // namespace rtype::game
