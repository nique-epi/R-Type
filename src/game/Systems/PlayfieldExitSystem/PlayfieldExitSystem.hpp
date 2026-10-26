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
 * An enemy is out when its Position is more than BYDO_EXIT_MARGIN beyond the
 * left edge, a missile of an enemy when its Position is more than half of
 * ENEMY_MISSILE_WIDTH beyond that edge, and a missile of a player when its
 * Position is more than half of PLAYER_MISSILE_WIDTH beyond the right edge:
 * exactly the margin beyond is still in. The enemies and their missiles fly
 * left, so neither is out past the right edge, where the Bydos appear. Only
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
