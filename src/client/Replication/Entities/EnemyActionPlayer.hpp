#pragma once

#include "EnemyAction.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

class ISoundPlayer;

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
 * It also plays the sound the sheet names for that attack and phase, if it
 * names one (see EntitySounds). Call it from the frame loop only, outside any
 * forEach of the registry. The registry and the sound player must outlive the
 * player.
 */
class EnemyActionPlayer {
 public:
  EnemyActionPlayer(engine::ComponentRegistry& components,
                    ISoundPlayer& sounds);

  /** @brief Plays the animation and the sound @p action announces; see the
   * class. */
  void apply(const network::EnemyAction& action);

 private:
  engine::ComponentRegistry* components_;
  ISoundPlayer* sounds_;
};

}  // namespace rtype::client
