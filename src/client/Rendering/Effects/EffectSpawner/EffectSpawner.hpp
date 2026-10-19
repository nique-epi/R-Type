#pragma once

#include "Entity.hpp"
#include "EntityType.hpp"
#include "FlashPreference.hpp"
#include "Position.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::client {

struct EffectAnimations;

/**
 * @brief Plays the effects of a match: the explosion of an entity killed, the
 * muzzle flash of a missile that appears, and the flash of an entity hit.
 *
 * An effect is an entity of its own, in the Effects layer, centered on the
 * position given, whose animation plays once; AnimationSystem destroys it
 * after its last frame. Each effect has a version without flash, played while
 * the flash preference is FlashPreference::Reduced; the preference is read when
 * an effect starts. Every method adds components, so none may be called during
 * a forEach of the registry. The registries must outlive the spawner.
 */
class EffectSpawner {
 public:
  EffectSpawner(engine::EntityRegistry& entities,
                engine::ComponentRegistry& components);

  /** @brief Sets which version of the effects starts from now on: the
   * standard one, or the one without flash. FlashPreference::Full until
   * set. */
  void setFlashPreference(FlashPreference preference);

  /**
   * @brief Plays the explosion of an entity killed, centered where it died:
   * three fireballs for a ship, a burst of drops for a Bydo, an impact for a
   * missile. A value outside EntityType plays nothing.
   */
  void playExplosion(game::EntityType type, game::Position center);

  /**
   * @brief Plays the appearance effect of an entity, centered where it
   * appears: the muzzle flash of a missile of a player or of an enemy. A ship
   * or a Bydo appears without effect.
   */
  void playAppearance(game::EntityType type, game::Position center);

  /**
   * @brief Draws an entity hit white for HIT_FLASH_DURATION, or shaded when
   * the player reduced flashes; a second hit starts the flash over. An entity
   * that is not alive, or has no Sprite, is left alone.
   */
  void flashHit(engine::Entity entity);

 private:
  /** @brief Creates the entity of an effect, centered on `center`, showing
   * the version the flash preference asks for. */
  void spawn(const EffectAnimations& effect, game::Position center);

  engine::EntityRegistry* entities_;
  engine::ComponentRegistry* components_;
  FlashPreference flashes_ = FlashPreference::Full;
};

}  // namespace rtype::client
