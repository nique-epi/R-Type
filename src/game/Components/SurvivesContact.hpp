#pragma once

namespace rtype::game {

/** @brief Marker of an enemy a ship cannot destroy by touching it: the ship
 * loses a life, the enemy stays. Declared by `[gameplay.survives_contact]`;
 * LifeKeeper reads it. */
struct SurvivesContact {};

}  // namespace rtype::game
