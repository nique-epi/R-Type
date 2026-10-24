#pragma once

#include "EffectTools.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Position.hpp"

namespace rtype::client {

/**
 * @brief Plays the effects of a shot that lands: a hit on an entity (T1) and
 * the impact of a missile destroyed. Both throw three sparks towards the
 * shooter: the players shoot to the right, the enemies to the left. Every
 * method adds components, so none may be called during a forEach of the
 * registry.
 */
class HitEffects {
 public:
  explicit HitEffects(EffectTools tools);

  /**
   * @brief Plays T1 for an entity hit: white for HIT_FLASH_DURATION, or
   * shaded; then, when it has an EntityType and a Position, drawn a pixel
   * along the shot for KNOCKBACK_DURATION and three sparks towards its
   * shooter. A second hit starts it over. An entity that has no Sprite is
   * left alone.
   */
  void playHit(engine::Entity entity);

  /** @brief Plays the impact of a missile destroyed, centered where it died,
   * and three sparks towards its shooter. */
  void playImpact(game::EntityType type, game::Position center);

 private:
  /** @brief Flashes the silhouette of an entity hit, or shades it. */
  void flash(engine::Entity entity) const;

  /** @brief Bursts three sparks towards the shooter of a shot that touched
   * an entity of that type. */
  void burstSparks(game::EntityType type, game::Position center) const;

  EffectTools tools_;
};

}  // namespace rtype::client
