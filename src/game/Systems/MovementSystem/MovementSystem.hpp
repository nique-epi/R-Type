#pragma once

#include <vector>
#include "Bounds.hpp"
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Moves every entity that has a Position, an Intent and a ShipMotion or
 * a SteadyMotion; an entity with neither does not move.
 *
 * An entity with a ShipMotion moves by the directions its Intent holds, as
 * shipAfter() computes it, whichever controller wrote the Intent; it is then
 * stopped against the box of every Obstacle whose contact is blocking
 * (blockedByObstacles()), and kept inside the playfield again
 * (stoppedAtTheEdges()): a ship a wall pushes past an edge stays over the wall,
 * which CollisionRules then turns into a contact.
 *
 * An entity with a SteadyMotion moves by its top speed times the share its
 * Intent asks for on each axis, a share beyond FULL_STEERING counting as
 * FULL_STEERING, over the elapsed simulation time: it depends on the time
 * given, never on how many frames were rendered. It is not kept inside the
 * playfield, so an enemy or a missile can leave it.
 */
class MovementSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  /** @brief The boxes of the blocking obstacles of the update in progress,
   * kept so their room is reused from one update to the next. */
  std::vector<engine::Bounds> blockingBoxes_;
};

}  // namespace rtype::game
