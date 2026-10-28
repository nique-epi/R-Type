#include "EffectSpawner.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "ComponentRegistry.hpp"
#include "EffectPreferences.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "PilotShadeConstants.hpp"
#include "PilotShades.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

EffectSpawner::EffectSpawner(engine::EntityRegistry& entities,
                             engine::ComponentRegistry& components,
                             std::uint32_t seed)
    : shades_(STANDARD_PILOT_SHADES),
      pixels_(entities, components, seed),
      tools_{.entities = entities,
             .components = components,
             .gate = gate_,
             .pixels = pixels_,
             .camera = camera_,
             .shades = shades_},
      gains_(tools_),
      hits_(tools_),
      shots_(tools_),
      kills_(tools_, gains_),
      pilots_(tools_),
      cues_(tools_) {}

void EffectSpawner::advance(engine::Duration elapsed,
                            EffectPreferences preferences) {
  gate_.advance(elapsed, preferences.flashes);
  shades_ = preferences.shades;
  soundCues_ = preferences.soundCues;
  kills_.advance(elapsed);
  gains_.advance(elapsed);
  camera_.advance(elapsed);
  pilots_.advance(elapsed);
}

void EffectSpawner::playExplosion(game::EntityType type,
                                  game::Position center) {
  switch (type) {
    case game::EntityType::Enemy:
      kills_.playKill(center);
      return;
    case game::EntityType::Player:
      kills_.playShipExplosion(center);
      return;
    case game::EntityType::PlayerMissile:
    case game::EntityType::EnemyMissile:
      hits_.playImpact(type, center);
      return;
  }
}

void EffectSpawner::playShipDown(engine::Entity ship) {
  if (soundCues_) {
    cues_.showPilotHit(ship);
  }
  kills_.playShipDown(ship);
}

void EffectSpawner::playAppearance(game::EntityType type,
                                   game::Position center) {
  switch (type) {
    case game::EntityType::PlayerMissile:
      shots_.playPlayerShot(center);
      return;
    case game::EntityType::EnemyMissile:
      shots_.playEnemyShot(center);
      if (soundCues_) {
        cues_.showEnemyShot(center);
      }
      return;
    case game::EntityType::Player:
    case game::EntityType::Enemy:
      return;
  }
}

void EffectSpawner::playArrival(engine::Entity ship) {
  pilots_.playArrival(ship);
}

void EffectSpawner::playHit(engine::Entity entity) { hits_.playHit(entity); }

void EffectSpawner::setInvulnerable(engine::Entity ship, bool invulnerable) {
  pilots_.setInvulnerable(ship, invulnerable);
}

void EffectSpawner::noteScore(int pilotColor, std::uint32_t score) {
  gains_.noteScore(pilotColor, score);
}

sf::Vector2i EffectSpawner::cameraOffset() const { return camera_.offset(); }

engine::Duration EffectSpawner::sinceStart() const {
  return pilots_.sinceStart();
}

}  // namespace rtype::client
