#pragma once

namespace rtype::game {

/** @brief Marker of a missile of an enemy that a missile of a player shoots
 * down: both disappear (see DamageRules). World::spawn gives it to the sheets
 * SHOOTABLE_MISSILE_SHEETS names; CollisionRules and LagCompensatedHitSystem
 * read it. */
struct Shootable {};

}  // namespace rtype::game
