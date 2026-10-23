#include "EntitySprites.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <optional>
#include "AnimatedSprite.hpp"
#include "Animation.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The animation of an entity type, whose first frame is the sprite
 * of the type, or nullptr for a value outside EntityType. */
const Animation* entityAnimationOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return &PLAYER_SHIP_ANIMATION;
    case game::EntityType::Enemy:
      return &BYDO_ANIMATION;
    case game::EntityType::PlayerMissile:
      return &PLAYER_MISSILE_ANIMATION;
    case game::EntityType::EnemyMissile:
      return &ENEMY_MISSILE_ANIMATION;
  }
  return nullptr;
}

/** @returns The layer an entity type is drawn in: a ship and the missile of a
 * player in front of the effects, the missile of an enemy in front of
 * everything of the playfield, a Bydo behind the effects. */
game::Layer layerOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
    case game::EntityType::PlayerMissile:
      return game::Layer::Players;
    case game::EntityType::EnemyMissile:
      return game::Layer::EnemyMissiles;
    case game::EntityType::Enemy:
      break;
  }
  return game::Layer::Enemies;
}

/** @returns The animation with its elapsed time set to the offset of a network
 * id: the fractional part of networkId times LOOP_START_SPREAD, as a share of
 * one cycle. */
SpriteAnimation startedAt(const Animation& animation, std::uint32_t networkId) {
  const double shareOfCycle =
      std::fmod(static_cast<double>(networkId) * LOOP_START_SPREAD, 1.0);
  return SpriteAnimation{
      .animation = animation,
      .elapsed = std::chrono::duration_cast<engine::Duration>(
          animation.cycle() * shareOfCycle)};
}

/** @returns The sprite of an entity showing @p frame of the entity sheet, in
 * @p layer. */
game::Sprite entitySpriteShowing(game::SpriteFrame frame, game::Layer layer) {
  return game::Sprite{
      .assetId = ENTITY_SHEET_ID, .frame = frame, .layer = layer};
}

}  // namespace

std::optional<game::Sprite> spriteOf(game::EntityType type) {
  const Animation* animation = entityAnimationOf(type);
  if (animation == nullptr) {
    return std::nullopt;
  }
  return entitySpriteShowing(animation->frames.front(), layerOf(type));
}

std::optional<SpriteAnimation> animationOf(game::EntityType type,
                                           std::uint32_t networkId) {
  const Animation* animation = entityAnimationOf(type);
  if (animation == nullptr) {
    return std::nullopt;
  }
  return startedAt(*animation, networkId);
}

std::optional<AnimatedSprite> animatedSpriteOf(game::EntityType type,
                                               std::uint32_t networkId) {
  const Animation* animation = entityAnimationOf(type);
  if (animation == nullptr) {
    return std::nullopt;
  }
  const SpriteAnimation started = startedAt(*animation, networkId);
  return AnimatedSprite{
      .sprite = entitySpriteShowing(started.animation.frameAt(started.elapsed)
                                        .value_or(animation->frames.front()),
                                    layerOf(type)),
      .animation = started};
}

sf::IntRect rectangleOf(game::SpriteFrame frame) {
  return {{frame.left, frame.top}, {frame.width, frame.height}};
}

game::SpriteFrame inPlayerColorRow(game::SpriteFrame frame, int color) {
  frame.top = PLAYER_SHIP_FRAME.top +
              (color * (PLAYER_SHIP_FRAME.height + SPRITE_SPACING));
  return frame;
}

}  // namespace rtype::client
