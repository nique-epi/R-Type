#include "EffectSpawner.hpp"
#include <optional>
#include "Animation.hpp"
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "FlashPreference.hpp"
#include "HitFlash.hpp"
#include "Layer.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The explosion of an entity type, or nullptr for a value outside
 * EntityType. */
const EffectAnimations* explosionOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return &SHIP_EXPLOSION;
    case game::EntityType::Enemy:
      return &BYDO_EXPLOSION;
    case game::EntityType::PlayerMissile:
    case game::EntityType::EnemyMissile:
      return &IMPACT;
  }
  return nullptr;
}

/** @returns The appearance effect of an entity type, or nullptr for a type
 * that appears without effect and for a value outside EntityType. */
const EffectAnimations* appearanceOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::PlayerMissile:
      return &PLAYER_MUZZLE_FLASH;
    case game::EntityType::EnemyMissile:
      return &ENEMY_MUZZLE_FLASH;
    case game::EntityType::Player:
    case game::EntityType::Enemy:
      break;
  }
  return nullptr;
}

}  // namespace

EffectSpawner::EffectSpawner(engine::EntityRegistry& entities,
                             engine::ComponentRegistry& components)
    : entities_(&entities), components_(&components) {}

void EffectSpawner::setFlashPreference(FlashPreference preference) {
  flashes_ = preference;
}

void EffectSpawner::playExplosion(game::EntityType type,
                                  game::Position center) {
  if (const EffectAnimations* explosion = explosionOf(type);
      explosion != nullptr) {
    spawn(*explosion, center);
  }
}

void EffectSpawner::playAppearance(game::EntityType type,
                                   game::Position center) {
  if (const EffectAnimations* appearance = appearanceOf(type);
      appearance != nullptr) {
    spawn(*appearance, center);
  }
}

void EffectSpawner::flashHit(engine::Entity entity) {
  if (!components_->has<game::Sprite>(entity)) {
    return;
  }
  components_->add(entity, HitFlash{.tone = flashes_ == FlashPreference::Reduced
                                                ? SpriteTone::Shade
                                                : SpriteTone::Flash,
                                    .remaining = HIT_FLASH_DURATION});
}

void EffectSpawner::spawn(const EffectAnimations& effect,
                          game::Position center) {
  const Animation& animation = flashes_ == FlashPreference::Reduced
                                   ? effect.withoutFlash
                                   : effect.standard;
  const std::optional<game::SpriteFrame> firstFrame =
      animation.frameAt(engine::Duration::zero());
  if (!firstFrame.has_value()) {
    return;
  }
  const engine::Entity entity = entities_->create();
  components_->add(entity, center);
  components_->add(entity, game::Sprite{.assetId = EFFECT_SHEET_ID,
                                        .frame = *firstFrame,
                                        .layer = game::Layer::Effects});
  components_->add(entity,
                   SpriteAnimation{.animation = animation,
                                   .elapsed = engine::Duration::zero()});
}

}  // namespace rtype::client
