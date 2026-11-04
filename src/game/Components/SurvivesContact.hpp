#pragma once

namespace rtype::game {

/** @brief Marker of an enemy a ship cannot destroy by touching it: the ship
 * loses its life, the enemy stays. Read by LifeKeeper. */
struct SurvivesContact {};

}  // namespace rtype::game
