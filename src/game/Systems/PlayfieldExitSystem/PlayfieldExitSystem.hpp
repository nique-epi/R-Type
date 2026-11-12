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
 * left edge. A missile is out when its Position is more than half its
 * CollisionBox beyond an edge it flies towards: the left or the right edge as
 * the horizontal share of its Intent says, the top or the bottom one as the
 * vertical share does; exactly half beyond is still in. A missile no pilot
 * has steered yet flies the way of its side, right for a player, left for an
 * enemy. A missile without a CollisionBox is out as soon as its Position
 * passes such an edge. An obstacle is out as a missile flying left is, once
 * more than half its CollisionBox is beyond the left edge: a wide wall stays
 * while a part of it shows. An enemy, or a missile flying left, past the right
 * edge, where the enemies appear, is not out. Only
 * entities that have a NetworkIdentity are removed. A ship is never removed by
 * leaving. A Boss is never removed by leaving either: its charge takes it
 * past the left edge and back. The bus must outlive this object.
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
