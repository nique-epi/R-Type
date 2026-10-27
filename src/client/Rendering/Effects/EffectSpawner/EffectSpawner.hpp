#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "CameraShake.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "FlashGate.hpp"
#include "FlashPreference.hpp"
#include "GainFigures.hpp"
#include "HitEffects.hpp"
#include "KillEffects.hpp"
#include "PilotEffects.hpp"
#include "PixelEffects.hpp"
#include "Position.hpp"
#include "ShotEffects.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief Plays the effects of a game on a ladder of intensity: a shot (T0), a
 * hit (T1), a Bydo destroyed (T2), every fifth kill of a chain (T3), a ship
 * destroyed (T4); and the arrival of a ship, its invulnerability, and the
 * figure of each gain. It is what the replicator and the game screen call;
 * each family of effects is a class it holds (HitEffects, ShotEffects,
 * KillEffects, PilotEffects, GainFigures), all playing with the same
 * EffectTools.
 *
 * An effect of the sheet is an entity of its own, in the Effects layer,
 * centered on the position given, whose animation plays once; AnimationSystem
 * destroys it after its last frame (see spawnSheetEffect()).
 *
 * Every effect that flashes asks the FlashBudget first, through the
 * FlashGate, and plays its version without flash when refused. While the
 * flash preference is FlashPreference::Reduced, nothing flashes, blinks,
 * shakes, recoils or vibrates: the versions without flash play, a hit is
 * shaded, the particles lose the first color of their ramp, an invulnerable
 * ship is dithered all along and outlined, the figure of a gain stays shown
 * instead of blinking, and an arrival shows no white. The
 * preference is the one the last advance() gave, read when an effect starts:
 * an effect already playing keeps its version, but an invulnerability, which
 * follows the preference at every advance().
 *
 * advance() moves on what counts time: the game calls it every frame, before
 * applying the messages of the frame. Every other method but cameraOffset()
 * and sinceStart() adds components, so none may be called during a forEach
 * of the registry. The registries must outlive the spawner. Its families
 * hold references to its members, so it is never copied nor moved.
 */
class EffectSpawner {
 public:
  /** @param seed Seed of the draws of the particles: std::random_device in
   * a game, a fixed value in a test. */
  EffectSpawner(engine::EntityRegistry& entities,
                engine::ComponentRegistry& components, std::uint32_t seed);

  EffectSpawner(const EffectSpawner&) = delete;
  EffectSpawner& operator=(const EffectSpawner&) = delete;
  EffectSpawner(EffectSpawner&&) = delete;
  EffectSpawner& operator=(EffectSpawner&&) = delete;
  ~EffectSpawner() = default;

  /** @brief Moves on the flash budget, the chain of kills, the kills and
   * gains waiting for each other, the camera shake and the time since the
   * game started, and sets which version of the effects starts from now on:
   * the standard one, or the one without flash. */
  void advance(engine::Duration elapsed, FlashPreference preference);

  /**
   * @brief Plays the explosion of an entity killed, centered where it died:
   * T2 or T3 for a Bydo (see KillEffects::playKill()), the impact of a
   * missile and three sparks towards its shooter, the three fireballs of a
   * ship alone (playShipDown() plays the whole of T4). A value outside
   * EntityType plays nothing.
   */
  void playExplosion(game::EntityType type, game::Position center);

  /** @brief Plays T4 for a ship destroyed (see KillEffects::playShipDown()).
   * Call it before the ship is destroyed. */
  void playShipDown(engine::Entity ship);

  /**
   * @brief Plays the appearance of an entity, centered where it appears: the
   * muzzle flash of a missile (see ShotEffects). A ship or a Bydo appears
   * without effect: see playArrival() for a ship.
   */
  void playAppearance(game::EntityType type, game::Position center);

  /** @brief Plays the arrival of a ship (see PilotEffects::playArrival()). */
  void playArrival(engine::Entity ship);

  /** @brief Plays T1 for an entity hit (see HitEffects::playHit()). */
  void playHit(engine::Entity entity);

  /** @brief Shows that a ship cannot be hit, or no longer can't (see
   * PilotEffects::setInvulnerable()). */
  void setInvulnerable(engine::Entity ship, bool invulnerable);

  /** @brief Notes the score a world state gives the pilot of a color; a
   * score that grew shows as the figure of a gain where a Bydo died (see
   * GainFigures). */
  void noteScore(int pilotColor, std::uint32_t score);

  /** @returns How many pixels of the canvas the sky and the entities are
   * drawn away from their place: the camera shake, (0, 0) at rest. */
  [[nodiscard]] sf::Vector2i cameraOffset() const;

  /** @returns The time advance() moved on since the spawner was built: the
   * time since the start of the game. */
  [[nodiscard]] engine::Duration sinceStart() const;

 private:
  FlashGate gate_;
  CameraShake camera_;
  PixelEffects pixels_;
  EffectTools tools_;
  GainFigures gains_;
  HitEffects hits_;
  ShotEffects shots_;
  KillEffects kills_;
  PilotEffects pilots_;
};

}  // namespace rtype::client
