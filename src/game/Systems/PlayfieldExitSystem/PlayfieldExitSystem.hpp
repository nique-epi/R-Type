#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Destroys every entity that went out of the playfield for good, and
 * announces each one with an EntityDestruction Removed.
 *
 * An enemy is out when its Position is more than ENEMY_EXIT_MARGIN beyond the
 * left edge, a missile of an enemy when its Position is more than half the
 * width of its CollisionBox beyond that edge, and a missile of a player when
 * its Position is more than half that width beyond the right edge: exactly
 * the margin beyond is still in. A missile without a CollisionBox is out as
 * soon as its Position passes the edge. The enemies and their missiles fly
 * left, so neither is out past the right edge, where the enemies appear. Only
 * entities that have a NetworkIdentity are removed. A ship is never removed by
 * leaving. The bus must outlive this object.
 */
class PlayfieldExitSystem final : public engine::ISystem {
 public:
  explicit PlayfieldExitSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  engine::EventBus& events_;
};

}  // namespace rtype::game
