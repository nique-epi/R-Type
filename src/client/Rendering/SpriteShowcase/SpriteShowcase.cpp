#include "SpriteShowcase.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "BackgroundConstants.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteShowcaseConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

game::Position positionOf(const ShowcaseEntity& shown) {
  return game::Position{
      .x = game::PLAYFIELD_WIDTH * shown.shareOfPlayfieldWidth,
      .y = game::PLAYFIELD_HEIGHT * shown.shareOfPlayfieldHeight};
}

/** @returns The middle of the left side of a Bydo standing at `center`, where
 * a missile coming from the left bursts: half its width away, in logical
 * units. */
game::Position leftSideOfBydo(game::Position center) {
  return game::Position{.x = center.x - (static_cast<float>(BYDO_FRAME.width) *
                                         HALF * BACKGROUND_PIXEL_SIZE),
                        .y = center.y};
}

std::uint32_t networkIdOf(std::size_t place) {
  return FIRST_SHOWCASE_NETWORK_ID + static_cast<std::uint32_t>(place);
}

}  // namespace

SpriteShowcase::SpriteShowcase(engine::EntityRegistry& entities,
                               engine::ComponentRegistry& components,
                               EffectSpawner& effects)
    : entities_(&entities), components_(&components), effects_(&effects) {
  shown_.reserve(SPRITE_SHOWCASE.size());
  for (std::size_t place = 0; place < SPRITE_SHOWCASE.size(); ++place) {
    shown_.push_back(spawn(place));
  }
}

void SpriteShowcase::advance(engine::Duration elapsed) {
  engine::Duration remaining = std::min(elapsed, SHOWCASE_LOOP);
  while (remaining > engine::Duration::zero()) {
    const engine::Duration covered =
        std::min(remaining, SHOWCASE_LOOP - loopTime_);
    playStepsUntil(loopTime_ + covered);
    loopTime_ = (loopTime_ + covered) % SHOWCASE_LOOP;
    remaining -= covered;
  }
}

void SpriteShowcase::playStepsUntil(engine::Duration until) {
  for (const ShowcaseStep& step : SHOWCASE_TIMELINE) {
    if (loopTime_ < step.at && step.at <= until) {
      play(step.cue);
    }
  }
}

void SpriteShowcase::play(ShowcaseCue cue) {
  switch (cue) {
    case ShowcaseCue::Shots:
      flashMuzzle(SHOWCASE_PLAYER_MISSILE);
      flashMuzzle(SHOWCASE_ENEMY_MISSILE);
      return;
    case ShowcaseCue::Hit:
      hit(SHOWCASE_FIRST_BYDO);
      return;
    case ShowcaseCue::BydoExplodes:
      explode(SHOWCASE_FIRST_BYDO);
      return;
    case ShowcaseCue::ShipExplodes:
      explode(SHOWCASE_SHIP);
      return;
    case ShowcaseCue::Return:
      bringBack(SHOWCASE_SHIP);
      bringBack(SHOWCASE_FIRST_BYDO);
      return;
  }
}

engine::Entity SpriteShowcase::spawn(std::size_t place) {
  const ShowcaseEntity& shown = SPRITE_SHOWCASE.at(place);
  const engine::Entity entity = entities_->create();
  components_->add(entity, positionOf(shown));
  std::optional<game::Sprite> sprite = spriteOf(shown.type);
  const std::optional<SpriteAnimation> animation =
      animationOf(shown.type, networkIdOf(place));
  if (sprite.has_value() && animation.has_value()) {
    sprite->frame = animation->animation.frameAt(animation->elapsed)
                        .value_or(sprite->frame);
    components_->add(entity, *sprite);
    components_->add(entity, *animation);
  }
  return entity;
}

void SpriteShowcase::flashMuzzle(std::size_t place) {
  const ShowcaseEntity& missile = SPRITE_SHOWCASE.at(place);
  effects_->playAppearance(missile.type, positionOf(missile));
}

void SpriteShowcase::hit(std::size_t place) {
  const ShowcaseEntity& bydo = SPRITE_SHOWCASE.at(place);
  effects_->playExplosion(game::EntityType::PlayerMissile,
                          leftSideOfBydo(positionOf(bydo)));
  effects_->flashHit(shown_.at(place));
}

void SpriteShowcase::explode(std::size_t place) {
  const engine::Entity entity = shown_.at(place);
  if (!entities_->isAlive(entity)) {
    return;
  }
  const ShowcaseEntity& shown = SPRITE_SHOWCASE.at(place);
  effects_->playExplosion(shown.type, positionOf(shown));
  components_->destroy(entity);
}

void SpriteShowcase::bringBack(std::size_t place) {
  if (!entities_->isAlive(shown_.at(place))) {
    shown_.at(place) = spawn(place);
  }
}

}  // namespace rtype::client
