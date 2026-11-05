#pragma once

#include "EnemyAction.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief Plays on an enemy the animation its sheet gives to the attack and
 * the phase an EnemyAction announces.
 *
 * The entity is the one whose NetworkIdentity is the entity id of the
 * message, and the animation comes from the AppearanceReference it carries: an
 * entity the client does not have, or whose sheet gives no animation to that
 * attack and phase, is left as it is. The animation replaces the
 * SpriteAnimation of the entity from its first frame, and goes back to the idle
 * animation once it has ended when the sheet says so (see ActionAnimation).
 * Call it from the frame loop only, outside any forEach of the registry. The
 * registry must outlive the player.
 */
class EnemyActionPlayer {
 public:
  explicit EnemyActionPlayer(engine::ComponentRegistry& components);

  /** @brief Plays the animation @p action announces; see the class. */
  void apply(const network::EnemyAction& action);

 private:
  engine::ComponentRegistry* components_;
};

}  // namespace rtype::client
