#pragma once

#include <cstddef>
#include <vector>
#include "Entity.hpp"
#include "SpriteShowcaseConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::client {

class EffectSpawner;

/**
 * @brief The provisional showcase of the game screen: one entity of each type
 * and two more Bydos, animated, and a loop of SHOWCASE_LOOP that plays every
 * effect.
 *
 * In turn, both missiles flash as if just fired, the first Bydo is hit twice,
 * it explodes, the ship explodes, and both come back; then the loop starts
 * again. A step plays when the time crosses it; a step that crosses the end of
 * the loop plays the steps in their order, and one long step plays one loop at
 * most. The entities take the network ids FIRST_SHOWCASE_NETWORK_ID onwards,
 * in the order of SPRITE_SHOWCASE.
 *
 * Provisional: it stands in for the entities the server announces, and goes
 * away when the client creates them.
 */
class SpriteShowcase {
 public:
  /**
   * @param entities Entities of the game screen; must outlive the showcase.
   * @param components Their components; must outlive the showcase.
   * @param effects Spawner of the effects the loop plays; must outlive the
   * showcase.
   */
  SpriteShowcase(engine::EntityRegistry& entities,
                 engine::ComponentRegistry& components, EffectSpawner& effects);

  /**
   * @brief Moves the loop forward and plays the steps the time crosses. It
   * creates and destroys entities: never call it during a forEach.
   */
  void advance(engine::Duration elapsed);

 private:
  /** @brief Plays, in order, the steps after the current time of the loop and
   * up to `until`. */
  void playStepsUntil(engine::Duration until);

  void play(ShowcaseCue cue);

  /** @returns A new entity standing for SPRITE_SHOWCASE[place], with its
   * sprite and its animation. */
  engine::Entity spawn(std::size_t place);

  /** @brief Plays the muzzle flash of the missile at SPRITE_SHOWCASE[place]. */
  void flashMuzzle(std::size_t place);

  /** @brief Bursts an impact on the left side of the Bydo at
   * SPRITE_SHOWCASE[place], and flashes it when it is alive. */
  void hit(std::size_t place);

  /** @brief Plays the explosion of the entity at SPRITE_SHOWCASE[place] and
   * destroys it, unless it is already gone. */
  void explode(std::size_t place);

  /** @brief Spawns the entity at SPRITE_SHOWCASE[place] again, unless it is
   * alive. */
  void bringBack(std::size_t place);

  engine::EntityRegistry* entities_;
  engine::ComponentRegistry* components_;
  EffectSpawner* effects_;
  std::vector<engine::Entity> shown_;
  engine::Duration loopTime_{};
};

}  // namespace rtype::client
