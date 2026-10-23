#pragma once

#include "EffectTools.hpp"
#include "Entity.hpp"
#include "Position.hpp"

namespace rtype::client {

/**
 * @brief Plays the muzzle flash of a missile that appears, centered where it
 * appears. Every method adds components, so none may be called during a
 * forEach of the registry.
 */
class ShotEffects {
 public:
  explicit ShotEffects(EffectTools tools);

  /** @brief Plays T0 for a missile of a player: its muzzle flash follows the
   * nose of the nearest ship at its left, which recoils; without such a
   * ship, it stays where the missile appeared. */
  void playPlayerShot(game::Position center);

  /** @brief Plays the muzzle flash of a missile of an enemy. */
  void playEnemyShot(game::Position center);

 private:
  /** @brief Anchors the muzzle flash of a missile of a player that appears
   * at @p center to the nose of its shooter, which recoils; nothing without
   * a shooter. */
  void followShooter(engine::Entity muzzle, game::Position center) const;

  EffectTools tools_;
};

}  // namespace rtype::client
