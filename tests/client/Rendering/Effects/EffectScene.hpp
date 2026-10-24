#pragma once

#include "AnimationSystem.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "FlashPreference.hpp"
#include "HitFlashSystem.hpp"
#include "Position.hpp"
#include "SpriteEffectSystem.hpp"
#include "TimeConstants.hpp"

/**
 * @brief Registries with a spawner of effects, and the systems that play
 * them. What the tests read in the registries, the functions of
 * EffectSpawnerFixtures.hpp read from components().
 */
class EffectScene {
 public:
  EffectScene();

  rtype::client::EffectSpawner& effects();

  /** @brief Moves the animations on, and nothing else. */
  void play(rtype::engine::Duration elapsed);

  /** @brief Moves the spawner and the systems of the sprites on, as a frame
   * of the game screen does, with the flash preference of the player. */
  void advance(rtype::engine::Duration elapsed,
               rtype::client::FlashPreference preference);

  /** @returns A ship of the pilot of that color, as the replicator makes it,
   * at SHIP_CENTER. */
  rtype::engine::Entity shipOfPilot(int pilotColor);

  /** @returns A Bydo at that position, as the replicator makes it. */
  rtype::engine::Entity bydoAt(rtype::game::Position position);

  /** @returns A drawn entity, as a Bydo the player may hit. */
  rtype::engine::Entity target();

  rtype::engine::EntityRegistry& entities();
  rtype::engine::ComponentRegistry& components();

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
  rtype::client::EffectSpawner effects_;
  rtype::client::AnimationSystem animations_;
  rtype::client::HitFlashSystem hitFlashes_;
  rtype::client::SpriteEffectSystem spriteEffects_;
};
